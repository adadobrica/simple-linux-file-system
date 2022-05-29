#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

#define MAX_STRING_SIZE 256

void add_new_node(TreeNode **currentNode, TreeNode *data) {
    FolderContent *get_list = (FolderContent *)(*currentNode)->content;
    ListNode *new_node = malloc(sizeof(ListNode));

    new_node->info = malloc(sizeof(TreeNode));
    memcpy(new_node->info, data, sizeof(TreeNode));

    // add at the beginning of the list
    new_node->next = get_list->children->head;
    get_list->children->head = new_node;
}

FileTree createFileTree(char* rootFolderName) {
    FileTree NewFileTree;
    // create root
    NewFileTree.root = malloc(sizeof(TreeNode));
    NewFileTree.root->parent = NULL;

    // set root name
    NewFileTree.root->name = malloc(MAX_STRING_SIZE * sizeof(char));
    memcpy(NewFileTree.root->name, rootFolderName, strlen(rootFolderName) + 1);

    // set folder content
    TreeNode* currentFolder = NewFileTree.root;
    currentFolder->type = FOLDER_NODE;
    currentFolder->content = malloc(sizeof(FolderContent));
    FolderContent *newFolder = malloc(sizeof(FolderContent));
    newFolder->children = malloc(sizeof(List));
    newFolder->children->head = NULL;
    memcpy(currentFolder->content, newFolder, sizeof(FolderContent));
    free(newFolder);

    return NewFileTree;
}

void freeFileNode(TreeNode *node) {
    // free memory of a file node
    FileContent *get_content = (FileContent *)node->content;
    free(get_content->text);
    free(node->content);
    free(node->name);
    free(node);
}

void freeFolderNode(TreeNode *node) {
    // free memory of a folder node
    FolderContent *get_folder = (FolderContent *)node->content;
    ListNode *current = get_folder->children->head;

    // free every file and folder in it
    while (current) {
        ListNode *nextNode = current->next;
        if (current->info->type == FILE_NODE) {
            freeFileNode(current->info);
        } else {
            freeFolderNode(current->info);
        }
        free(current);
        current = nextNode;
    }
    free(get_folder->children);
    free(node->content);
    free(node->name);
    free(node);
}

void freeTree(FileTree fileTree) {
    // free root folder
    freeFolderNode(fileTree.root);
}


void ls(TreeNode* currentNode, char* arg) {
    int len = strlen(arg);

    // print content of current folder
    if (len == 0) {
        FolderContent *currentDirectory = (FolderContent*)currentNode->content;
        ListNode *current = currentDirectory->children->head;

        // parse through current folder and print
        while (current) {
            TreeNode *data = current->info;
            printf("%s\n", data->name);
            current = current->next;
        }
    } else {
        // find given folder/file name
        FolderContent *currentDirectory = (FolderContent*)currentNode->content;
        ListNode *current = currentDirectory->children->head;

        // iterate thorugh folder and print file/folder if exists
        while (current) {
            TreeNode *data = current->info;
            // print content of file/folder if found
            if (!strcmp(data->name, arg)) {
                // print file content
                if (data->type == FILE_NODE) {
                    FileContent *get_content = (FileContent *)data->content;
                    printf("%s: %s\n", data->name, get_content->text);

                } else if (data->type == FOLDER_NODE) {
                    // print folder content
                    FolderContent *get_content = (FolderContent *)data->content;
                    ListNode *currentFolderNode = get_content->children->head;

                    while (currentFolderNode) {
                        TreeNode *folderData = currentFolderNode->info;
                        printf("%s\n", folderData->name);
                        currentFolderNode = currentFolderNode->next;
                    }
                }
                return;
            }
            current = current->next;
        }
        // file/folder was not found
        printf("ls: cannot access '%s': No such file or directory\n", arg);
    }
}


void pwd(TreeNode* treeNode) {
	char path[MAX_STRING_SIZE][MAX_STRING_SIZE];
	int idx = 0;

	// current folder is root
	if (treeNode->parent == NULL) {
		printf("%s\n", treeNode->name);
		return;
	}

    // add path in array
	while (treeNode->parent) {
		strcpy(path[idx++], treeNode->name);
		treeNode = treeNode->parent;
	}
	strcpy(path[idx++], treeNode->name);
    // print path
	for (int i = idx - 1; i > 0; i--) {
		printf("%s/", path[i]);
	}
    printf("%s", path[0]);
}


TreeNode* cd(TreeNode* currentNode, char* path, int cont) {
    int num_paths = 0;
    char paths[MAX_STRING_SIZE][MAX_STRING_SIZE];

    // copy path in array to parse easier
    char *token = strtok(path, "/");
    while (token) {
        strcpy(paths[num_paths++], token);
        token = strtok(NULL, "/\n");
    }

    int i = -1;
    TreeNode *my_node;
    TreeNode* currentNodeCopy = currentNode;
    // iterate through directories to given path
    while (num_paths) {
        i++;
        num_paths--;
        FolderContent *get_folder = (FolderContent *)currentNode->content;
        ListNode *current = get_folder->children->head;
        my_node = NULL;

        // move to parent directory
        if (!strcmp(paths[i], PARENT_DIR) && currentNode->parent) {
            currentNode = currentNode->parent;
            my_node = currentNode;
            continue;
        }

        // search for given folder in current folder
        while (current) {
            if (!strcmp(paths[i], current->info->name)) {
                currentNode = current->info;
                my_node = currentNode;
                break;
            }
            current = current->next;
        }
        // file/folder was not found
        if (!my_node && cont == 1) {
            printf("cd: no such file or directory: %s", paths[i]);
            return currentNodeCopy;
        } else if (!my_node && !cont) {
            return my_node;
        }
    }
    // path was found successfully
    return my_node;
}


void recursiveTree(TreeNode* currentNode, int *level,
                    int *directories, int *files) {
    FolderContent *currentDirectory = (FolderContent*)currentNode->content;
    ListNode *current = currentDirectory->children->head;

    // iterate through folder nodes and print
    while (current) {
        TreeNode* node = (TreeNode*)current->info;
        // print file node
        if (node->type == FILE_NODE) {
            (*files)++;
            for (int i = 0; i < *level; i++)
                printf("\t");
            printf("%s\n", node->name);
        } else if (node->type == FOLDER_NODE) {
            // print folder node along with its children
            (*directories)++;
            for (int i = 0; i < *level; i++)
                printf("\t");
            printf("%s\n", node->name);
            (*level)++;
            recursiveTree(node, level, directories, files);
            (*level)--;
        }
        current = current->next;
    }
}


void tree(TreeNode* currentNode, char* arg) {
    int len = strlen(arg);

    // print content of current folder if there is no argument
    if (len == 0) {
        int level = 0, directories = 0, files = 0;
        recursiveTree(currentNode, &level, &directories, &files);
        printf("%d directories, %d files\n", directories, files);
        return;
    }
    // find folder
    TreeNode* folder = cd(currentNode, arg, 0);
    // exit if folder was not found
    if (!folder || folder->type == FILE_NODE) {
        printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
        return;
    }

    // print folder
    int level = 0, directories = 0, files = 0;
    recursiveTree(folder, &level, &directories, &files);
    printf("%d directories, %d files\n", directories, files);
}


void mkdir(TreeNode* currentNode, char* folderName) {
    FolderContent *get_folder = (FolderContent *)currentNode->content;
    ListNode *current = get_folder->children->head;

    // check if folder exists
    while (current) {
        TreeNode *data = current->info;
        if (!strcmp(data->name, folderName)) {
            printf("mkdir: cannot create directory '%s': File exists\n",
                    folderName);
            return;
        }
        current = current->next;
    }

    // create directory
    TreeNode *newDirectory = malloc(sizeof(TreeNode));
    newDirectory->parent = currentNode;
    newDirectory->type = FOLDER_NODE;
    newDirectory->name = malloc(MAX_STRING_SIZE * sizeof(char));
    memcpy(newDirectory->name, folderName, strlen(folderName) + 1);

    // initialize folder content
    newDirectory->content = malloc(sizeof(FolderContent));
    FolderContent *new_folder = malloc(sizeof(FolderContent));
    new_folder->children = malloc(sizeof(List));
    new_folder->children->head = NULL;
    memcpy(newDirectory->content, new_folder, sizeof(FolderContent));

    // add folder in list of nodes
    add_new_node(&currentNode, newDirectory);
    free(new_folder);
    free(newDirectory);
}


void rmrec(TreeNode* currentNode, char* resourceName) {
    FolderContent *get_content = (FolderContent *)currentNode->content;
    ListNode *current = get_content->children->head;

    // search for directory/file in current folder
    ListNode *prev = NULL;
    while (current) {
        // delete file
        if (!strcmp(current->info->name, resourceName) &&
             current->info->type == FILE_NODE) {
            if (!prev)
                get_content->children->head = current->next;
            else
                prev->next = current->next;
            freeFileNode(current->info);
            free(current);
            return;
        } else if (!strcmp(current->info->name, resourceName) &&
                   current->info->type == FOLDER_NODE) {
            // delete folder
            if (!prev)
                get_content->children->head = current->next;
            else
                prev->next = current->next;
            freeFolderNode(current->info);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }

    // file/folder was not found
    printf("rmrec: failed to remove '%s': No such file or directory\n",
            resourceName);
}


void rm(TreeNode* currentNode, char* fileName) {
    FolderContent *get_content = (FolderContent *)currentNode->content;
    ListNode *current = get_content->children->head;

    // search for file in current folder
    ListNode *prev = NULL;
    while (current) {
        // argument is a directory
        if (!strcmp(current->info->name, fileName) &&
             current->info->type == FOLDER_NODE) {
            printf("rm: cannot remove '%s': Is a directory\n",
                    fileName);
            return;
        } else if (!strcmp(current->info->name, fileName) &&
                    current->info->type == FILE_NODE) {
            // delete file
            if (!prev)
                get_content->children->head = current->next;
            else
                prev->next = current->next;
            freeFileNode(current->info);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }

    // file was not found
    printf("rm: failed to remove '%s': No such file or directory\n",
            fileName);
}


void rmdir(TreeNode* currentNode, char* folderName) {
    FolderContent *get_content =
    (FolderContent *)currentNode->content;
    ListNode *current = get_content->children->head;

    // search for directory in current folder
    ListNode *prev = NULL;
    while (current) {
        if (!strcmp(current->info->name, folderName) &&
                    current->info->type == FILE_NODE) {
            // argument is file
            printf("rmdir: failed to remove '%s': Not a directory\n",
                    folderName);
            return;
        } else if (!strcmp(current->info->name, folderName) &&
                    current->info->type == FOLDER_NODE) {
            // folder is not empty
            FolderContent* get_cur_content =
            (FolderContent*)current->info->content;
            if (get_cur_content->children->head) {
                printf("rmdir: failed to remove '%s': Directory not empty\n",
                        folderName);
                return;
            } else if (!get_cur_content->children->head) {
                // delete folder
                if (!prev)
                    get_content->children->head = current->next;
                else
                    prev->next = current->next;
                freeFolderNode(current->info);
                free(current);
                return;
            }
        }
        prev = current;
        current = current->next;
    }
    // folder was not found
    printf("rmdir: failed to remove '%s': No such file or directory\n",
            folderName);
}


void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
    FolderContent *get_content = (FolderContent *)currentNode->content;
    ListNode *current = get_content->children->head;

    // check if file exists and exit if so
    while (current) {
        if (current->info->type == FILE_NODE &&
            !strcmp(fileName, current->info->name)) {
            return;
        }
        current = current->next;
    }

    // initialize file tree node
    TreeNode* newFile = malloc(sizeof(TreeNode));
    newFile->parent = currentNode;
    newFile->type = FILE_NODE;
    newFile->name = malloc(MAX_STRING_SIZE * sizeof(char));
    memcpy(newFile->name, fileName, strlen(fileName) + 1);

    int content = strlen(fileContent);
    if (!content) {
        // add file with no content
        FileContent *new_content = malloc(sizeof(FileContent));
        new_content->text = malloc(sizeof(char));
        newFile->content = malloc(sizeof(FileContent));

        memcpy(new_content->text, fileContent, strlen(fileContent) + 1);
        memcpy(newFile->content, new_content, sizeof(FileContent));

        free(new_content);
    } else {
        // add content of file
        newFile->content = malloc(sizeof(FileContent));

        FileContent *new_content = malloc(sizeof(FileContent));
        new_content->text = malloc(MAX_STRING_SIZE * sizeof(char));

        memcpy(new_content->text, fileContent, strlen(fileContent) + 1);
        memcpy(newFile->content, new_content, sizeof(FileContent));
        free(new_content);
    }

    // adaugam fisierul in folderul curent
    add_new_node(&currentNode, newFile);
    free(newFile);
}


void cp(TreeNode* currentNode, char* source, char* destination) {
    // search for source file
    TreeNode* source_node =  cd(currentNode, source, 0);
    char cp_destination[MAX_STRING_SIZE];
    strcpy(cp_destination, destination);

    // check if path was valid
    if (!source_node) {
        printf("cp: -r not specified; omitting directory '%s'\n", source);
        return;
    } else if (source_node->type == FOLDER_NODE) {
        // check if source is file
        printf("cp: -r not specified; omitting directory '%s'\n", source);
        return;
    }

    // search for destination file/folder
    TreeNode* dest_node = cd(currentNode, destination, 0);
    if (!dest_node) {
        // folder not correct
        printf("cp: failed to access '%s': Not a directory", cp_destination);
        return;
    }

    // destination is existing file
    if (dest_node->type == FILE_NODE) {
        FileContent* dest_content = (FileContent*)dest_node->content;
        FileContent* source_content = (FileContent*)source_node->content;
        free(dest_content->text);
        dest_content->text = malloc(strlen(source_content->text) + 1);
        memcpy(dest_content->text, source_content->text,
                strlen(source_content->text) + 1);
        return;
    }
    // create new file with content from source
    FileContent* source_content = (FileContent*)source_node->content;
    touch(dest_node, source_node->name, source_content->text);
}


void removeNode(TreeNode* currentNode, char* source, char* destination) {
    int num_paths = 0;
    char sources[MAX_STRING_SIZE][MAX_STRING_SIZE];
    TreeNode* currentNodeCopy = currentNode;

    // copy path in array to parse easier
    char *token = strtok(source, "/");
    while (token) {
        strcpy(sources[num_paths++], token);
        token = strtok(NULL, "/\n");
    }

    int i = -1;
    ListNode* prev, *current;
    FolderContent *get_folder;
    // iterate through directories to given path
    while (num_paths) {
        i++;
        num_paths--;
        get_folder = (FolderContent *)currentNode->content;
        current = get_folder->children->head;
        prev = NULL;

        // move to parent directory
        if (!strcmp(sources[i], PARENT_DIR) && currentNode->parent) {
            currentNode = currentNode->parent;
            continue;
        }
        // search for given folder in current folder
        while (current) {
            if (!strcmp(sources[i], current->info->name)) {
                currentNode = current->info;
                break;
            }
            prev = current;
            current = current->next;
        }
    }
    // delete source from folder
    if (prev) {
        prev->next = current->next;
    } else {
        get_folder->children->head = current->next;
    }

    // delete file
    if (currentNode->type == FILE_NODE) {
        freeFileNode(currentNode);
        free(current);
    } else {    // move folder
        ListNode* source_node = current;
        currentNode = currentNodeCopy;
        // find destination list and place source folder
        num_paths = 0;
        char destinations[MAX_STRING_SIZE][MAX_STRING_SIZE];
        char *token = strtok(destination, "/");
        while (token) {
            strcpy(destinations[num_paths++], token);
            token = strtok(NULL, "/\n");
        }

        int i = -1;
        prev = NULL;
        current = NULL;
        get_folder = NULL;
        // iterate through directories to given path
        while (num_paths) {
            i++;
            num_paths--;
            get_folder = (FolderContent *)currentNode->content;
            current = get_folder->children->head;
            prev = NULL;

            // move to parent directory
            if (!strcmp(destinations[i], PARENT_DIR) && currentNode->parent) {
                currentNode = currentNode->parent;
                continue;
            }
            // search for given folder in current folder
            while (current) {
                if (!strcmp(destinations[i], current->info->name)) {
                    currentNode = current->info;
                    break;
                }
                prev = current;
                current = current->next;
            }
        }
        // set source folder at the biginning of list
        get_folder = (FolderContent *)currentNode->content;
        if (!get_folder->children->head) {
            source_node->next = NULL;
            get_folder->children->head = source_node;
            return;
        }
        // move folder in destination folder
        source_node->next = get_folder->children->head;
        get_folder->children->head = source_node;
    }
}


void mv(TreeNode* currentNode, char* source, char* destination) {
    // search for source file
    char cp_source[MAX_STRING_SIZE];
    strcpy(cp_source, source);
    char cp_destination[MAX_STRING_SIZE];
    strcpy(cp_destination, destination);
    TreeNode* source_node =  cd(currentNode, source, 0);
    // check if path was valid
    if (!source_node) {
        printf("mv: -r not specified; omitting directory '%s'\n",
                cp_source);
        return;
    }

    // search for destination file/folder
    TreeNode* dest_node = cd(currentNode, destination, 0);
    if (!dest_node) {
        // folder not correct
        printf("mv: failed to access '%s': Not a directory", cp_destination);
        return;
    }

    // source is file and destination is existing file
    if (dest_node->type == FILE_NODE && source_node->type == FILE_NODE) {
        // rewrite content of existing file and delete source file
        FileContent* dest_content = (FileContent*)dest_node->content;
        FileContent* source_content = (FileContent*)source_node->content;
        free(dest_content->text);
        dest_content->text = malloc(strlen(source_content->text) + 1);
        memcpy(dest_content->text, source_content->text,
               strlen(source_content->text) + 1);
        // delete source file
        removeNode(currentNode, cp_source, cp_destination);
    } else if (dest_node->type == FOLDER_NODE &&
                source_node->type == FILE_NODE) {
        // copy file in folder and delete file
        FileContent* source_content = (FileContent*)source_node->content;
        touch(dest_node, source_node->name, source_content->text);
        // delete source file
        removeNode(currentNode, cp_source, cp_destination);
    } else if (dest_node->type == FOLDER_NODE &&
               source_node->type == FOLDER_NODE) {
        // copy folder in new folder
        removeNode(currentNode, cp_source, cp_destination);
    }
}
