#include "node.h"
#ifndef COMMANDS_H_
#define COMMANDS_H_
#define PARAM_LEN 100
void printDirectoryMessage(Node *, char *);
void rmdir(Node *, char param[][PARAM_LEN], int);
void rm(Node *, char param[][PARAM_LEN], int);
void mkdir(Node *, char[][PARAM_LEN], int);
void touch(Node *, char[][PARAM_LEN], int);
void mv(Node *, char[][PARAM_LEN], int);
void cp(Node *, char[][PARAM_LEN], int);
void pwd(Node *);
void listPath(Node *);
Node *specialCD(Node *, char *);
Node *CD(Node *, char *);
void ls(Node *, char[][PARAM_LEN], int);
void listFD(Node *, int, char *);
#endif // !COMMANDS_H_