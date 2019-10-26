#ifndef NODE_H_
#define NODE_H_
typedef struct Node Node;
struct Node {
  char info;
  int asterisk;
  Node **children;
  Node *parent;
  int childrenNumber, childrenCapacity;
};
Node *newNode(char, int, Node *);
void deleteNode(Node *node);
#endif // !NODE_H_