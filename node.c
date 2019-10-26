#include "node.h"
#include <stdio.h>
#include <stdlib.h>
// functia prin care creez un nou node al trie-ului
Node *newNode(char c, int asterisk, Node *parent) {
  Node *tmp = (Node *)malloc(sizeof(Node));
  if (tmp == NULL) {
      fprintf(stderr, "MEMORY ERROR\n");
      return NULL;
  }
  tmp->info = c;
  tmp->childrenNumber = 0;
  tmp->childrenCapacity = 1;
  // marchez sfarsitul unui fisier
  tmp->asterisk = asterisk;
  tmp->parent = parent;
  // aloc dinamic memoria pentru copiii nodului curent
  tmp->children = (Node **)calloc(tmp->childrenCapacity, sizeof(Node));
  if (tmp -> children == NULL) {
      fprintf(stderr, "MEMORY ERROR\n");
      return NULL;
  }
  return tmp;
}
// functia de erase a unui nod
void deleteNode(Node *node) {
  free(node->children);
  free(node);
}