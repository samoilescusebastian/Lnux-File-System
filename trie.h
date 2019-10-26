#ifndef TRIE_H_
#define TRIE_H_
#include "node.h"
#include "trie.h"
typedef struct {
  Node *root;
} Trie;
Node *getBifurcation(Node *, char *, int *);
void removeChild(Node *, int);
int addKey(char *, Node *, int);
int isFile(Node *, char *);
void deleteTrie(Node *);
void printTrie(Node *);
Node *getDirectory(Node *);
Node *goToRoot(Node *);
Node *goToPath(Node *, char *);
#endif // !TRIE_H_
