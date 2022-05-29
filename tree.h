#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

typedef struct FileContent FileContent;
typedef struct FolderContent FolderContent;
typedef struct TreeNode TreeNode;
typedef struct FileTree FileTree;
typedef struct ListNode ListNode;
typedef struct List List;

enum TreeNodeType {
    FILE_NODE,
    FOLDER_NODE
};

struct FileContent {
    char* text;
};

struct FolderContent {
    List* children;
};

struct TreeNode {
    TreeNode* parent;
    char* name;
    enum TreeNodeType type;
    void* content;
};

struct FileTree {
    TreeNode* root;
};

struct ListNode {
    TreeNode* info;
    ListNode* next;
};

struct List {
    ListNode* head;
};


/* function that prints content of current folder if used
    without an argument;
    folder argument: children are printed;
    file argument: text content is printed*/
void ls(TreeNode* currentNode, char* arg);


/* function that prints the whole path to the root;*/
void pwd(TreeNode* treeNode);


/* function tries to follow the path given as argument and return
    the last final destination; it can also go backwards, towards
    the root by using .. 
    the argument 'cont' is used to distinguish when the command is
    given by the user and when it is used by other functions so
    that it can decide whether to print the error message or not*/
TreeNode* cd(TreeNode* currentNode, char* path, int cont);

/* function that prints content of a folder recursively*/
void recursiveTree(TreeNode* currentNode, int *level,
                    int *directories, int *files);


/* function that shows files hierarchy from current folder, 
   or given folder; it also prints at the end the total
   number of directories and files visited*/
void tree(TreeNode* currentNode, char* arg);


/* function that creates a new folder/directory with name
    given as parameter; if folder with given name already exists,
    an error message is printed*/
void mkdir(TreeNode* currentNode, char* folderName);


/* function that deletes file with given name as parameter;
    if file with given name is not found, or is not a file,
    an error message is be printed*/
void rm(TreeNode* currentNode, char* fileName);


/* function that deletes an empty folder with name given as
    parameter; if folder is not found, or is not empty, an
    error message is printed*/
void rmdir(TreeNode* currentNode, char* folderName);


/* function that deletes a file or folder with given name,
    regardless if empty or not; if file/folder is not found,
    an error message is printed*/
void rmrec(TreeNode* currentNode, char* resourceName);


/* function that creates a file with given name in current folder;
    if another file with the exact name exists, the command will 
    have no effect*/
void touch(TreeNode* currentNode, char* fileName, char* fileContent);


/* function that searches for node in given path and removes it by
    reseting the pointers and freeing the memory, if file, 
    or moves the node in destination folder list if folder;
    this function is a helper function for cp and mv*/
void removeNode(TreeNode* currentNode, char* source, char* destination);


/* functtion that copies file from source path to destination path;
   if destination is a folder, the file is copyied inside as a new
   file; if destination is an existing file, the content of the
   destination file is changed to that of the source file*/
void cp(TreeNode* currentNode, char* source, char* destination);


/* function that moves a file or directory as a whole, in another 
   folder from source path to destination path; a file can be mmoved
   in another existing file*/
void mv(TreeNode* currentNode, char* source, char* destination);


/* function that creates a file tree by allocating memory and
    initializing content*/
FileTree createFileTree(char* rootFolderName);


/* function that frees memory of a tree by freeing the root folder*/
void freeTree(FileTree fileTree);


/* function that adds a new node in a list of nodes of a folder;
    the nodes are always added at the beginning of the list*/
void add_new_node(TreeNode **currentNode, TreeNode *data);


/* function that frees the memory occupied by a file node*/
void freeFileNode(TreeNode *node);


/* function that deletes content of a folder node recursively*/
void freeFolderNode(TreeNode *node);
