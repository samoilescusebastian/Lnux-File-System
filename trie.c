#include "trie.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// sortarea crescatoarea a fiilor unui nod
void sort(Node **children, int length) {
  int sem;
  do {
    sem = 1;
    for (int i = 0; i < length - 1; i++) {
      if (children[i]->info > children[i + 1]->info) {
        Node *aux = children[i];
        children[i] = children[i + 1];
        children[i + 1] = aux;
        sem = 0;
      }
    }
  } while (sem == 0);
}
// adaugarea cheii la adresa data de un nod (type = 1 -> director)
//                                          (type = 2 -> fisier)
                                             
int addKey(char *key, Node *path, int type) {
  if (strlen(key) == 0) {
    if (type == 1) {
      for (int i = 0; i < path->childrenNumber; i++) {
        if (path->children[i]->info == '/') {
          return 1;
        }
      }
      if (path->info != '/') {
        // adaugdam caracterul specific unui director
        return addKey("/", path, type);
      } else {
        return 0;
      }
    } else if (type == 2) {
      if (path->asterisk == 1) {
        return 1;
      }
      path->asterisk = 1;
      return 0;
    }
  }
  Node *currNode = path;
  for (int i = 0; i < currNode->childrenNumber; i++) {
    if (currNode->children[i]->info == key[0]) {
      return addKey(key + 1, currNode->children[i], type);
    }
  }
  // realocare vector de copii
  if (path->childrenCapacity == path->childrenNumber) {
    path->childrenCapacity *= 2;
    path->children = (Node **)realloc(path->children,
                                      (sizeof(Node)) * path->childrenCapacity);
    if (path->children == NULL) {
      fprintf(stderr, "MEMORY ERROR\n");
      return -1;
    }
  }
  // creearea unui nou nod
  Node *tmp = newNode(key[0], 0, path);
  if (tmp == NULL) {
    fprintf(stderr, "MEMORY ERROR\n");
    return -1;
  }
  path->children[path->childrenNumber++] = tmp;
  sort(path->children, path->childrenNumber);
  return addKey(key + 1, tmp, type);
}
// verificarea daca la capatul unei caii date se afla un fisier
int isFile(Node *currentPath, char *fileAddress) {
  if (strlen(fileAddress) == 0) {
    if (currentPath->asterisk == 1) {
      return 1;
    }
    return 0;
  }
  for (int i = 0; i < currentPath->childrenNumber; i++) {
    if (currentPath->children[i]->info == fileAddress[0]) {
      return isFile(currentPath->children[i], fileAddress + 1);
    }
  }
  return 0;
}
// determinarea nodului de unde pot sterge o anumita cheie
// problematic din cauza partii comune a cheii cu alte chei
Node *getBifurcation(Node *currentPath, char *fileAddress, int *childNumber) {
  unsigned int j = 0;
  Node *bifurcation = NULL;
  while (j < strlen(fileAddress)) {
    if (currentPath->childrenNumber > 1) {
      bifurcation = currentPath;
      for (int i = 0; i < currentPath->childrenNumber; i++) {
        if (currentPath->children[i]->info == fileAddress[j]) {
          *childNumber = i;
          break;
        }
      }
    }
    for (int i = 0; i < currentPath->childrenNumber; i++) {
      if (currentPath->children[i]->info == fileAddress[j]) {
        currentPath = currentPath->children[i];
        break;
      }
    }
    j++;
  }
  if (bifurcation == NULL) {
    return currentPath;
  }
  if (currentPath->asterisk == 1 && currentPath->childrenNumber > 0) {
    *childNumber = -1;
    return currentPath;
  }
  return bifurcation;
}
// elimiarea unui anumit copil din lista de copii a unui nod
void removeChild(Node *node, int child) {
  for (int i = child; i < node->childrenNumber - 1; i++) {
    node->children[i] = node->children[i + 1];
  }
  node->childrenNumber--;
}
// stergerea unui sub-Trie(pleaca din nodul path)
void deleteTrie(Node *path) {
  for (int i = 0; i < path->childrenNumber; i++) {
    deleteTrie(path->children[i]);
  }
  deleteNode(path);
}
// obtinere director parinte
Node *getDirectory(Node *path) {
  while (path->info != '/') {
    path = path->parent;
  }
  return path;
}
// deplasare catre radacina
Node *goToRoot(Node *path) {
  while (path->parent != NULL) {
    path = path->parent;
  }
  return path;
}
// deplasare catre o anumita cale(newPath) pornind de la un anumit nod(path)
Node *goToPath(Node *path, char *newPath) {
  if (strlen(newPath) > 0) {
    for (int i = 0; i < path->childrenNumber; i++) {
      if (path->children[i]->info == newPath[0]) {
        return goToPath(path->children[i], newPath + 1);
      }
    }
  } else {
    for (int i = 0; i < path->childrenNumber; i++) {
      if (path->children[i]->info == '/') {
        return path->children[i];
      }
    }
    return NULL;
  }
  return NULL;
}
