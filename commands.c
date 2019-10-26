#include "commands.h"
#include "node.h"
#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PATH_L 100
// elimin din calea unui fisier "falsele" sfarsituri
void removeAsterisk(char *path) {
  for (unsigned int i = 0; i < strlen(path) - 1; i++) {
    if (path[i] == '*') {
      memmove(path + i, path + i + 1, strlen(path + i));
    }
  }
}
void printDirectoryMessage(Node *currentNode, char *line) {
  if (isFile(currentNode, line)) {
    fprintf(stderr, "%s: Not a directory\n", line);
  } else {
    fprintf(stderr, "%s: No such file or directory\n", line);
  }
}
// sparg calea completa in calea pana la director/fisier si numele
// directorului/fisierului
void splitFullPath(char *pathToDir, char *fullPath) {
  char *p = strrchr(fullPath, '/');
  int length = p - fullPath;
  if (p != NULL) {
    strncpy(pathToDir, fullPath, length);
    pathToDir[length] = '\0';
    memmove(fullPath, fullPath + length + 1, strlen(fullPath));
  }
}
void mkdir(Node *path, char param[][PARAM_LEN], int paramNumber) {
  char pathToDir[PATH_L];
  Node *currentNode;
  char fullPath[PATH_L];
  for (int i = 0; i < paramNumber; i++) {
    pathToDir[0] = '\0';
    // retin calea completa pentru a afisa eventuala eroare
    strcpy(fullPath, param[i]);
    splitFullPath(pathToDir, param[i]);
    // tratare caz special (deplasare in radacina)
    if (fullPath[0] == '/' && strlen(pathToDir) == 0) {
      pathToDir[0] = '/';
      pathToDir[1] = '\0';
    }
    Node *result = CD(path, pathToDir);
    // cale invalida
    if (result == NULL) {
      fprintf(stderr, "%s: No such file or directory\n", fullPath);
      continue;
    }
    currentNode = result;
    // adaugare in trie a cheii corespunzatoare directorului(type = 1)
    if (addKey(param[i], currentNode, 1)) {
      fprintf(stderr, "%s: already exists\n", fullPath);
    }
  }
}
void touch(Node *path, char param[][PARAM_LEN], int paramNumber) {
  char pathToDir[PATH_L] = "";
  Node *currentNode;
  for (int i = 0; i < paramNumber; i++) {
    pathToDir[0] = '\0';
    splitFullPath(pathToDir, param[i]);
    Node *result = CD(path, pathToDir);
    // cale invalida
    if (result == NULL) {
      continue;
    }
    currentNode = result;
    // adaugare in trie a cheii corespunzatoare fisierului(type = 2)
    if (addKey(param[i], currentNode, 2)) {
      fprintf(stdout, "%s: already exists\n", param[i]);
    }
  }
}
void mv(Node *path, char param[][PARAM_LEN], int paramNumber) {
  char pathToFIle[PATH_L] = "";
  Node *currentNode = path, *newNode = path;
  char *newPath = param[paramNumber - 1];
  char fullPath[PATH_L];
  // ma plasez in fisierul destinatie
  newNode = CD(newNode, newPath);
  for (int i = 0; i < paramNumber - 1; i++) {
    strcpy(fullPath, param[i]);
    splitFullPath(pathToFIle, param[i]);
    // ma deplasez pana la calea fisierului pe care vreau sa il mut
    currentNode = CD(path, pathToFIle);
    if (currentNode == NULL) {
      printDirectoryMessage(currentNode, pathToFIle);
    }
    // verificare daca e fisier
    if (!isFile(currentNode, param[i])) {
      fprintf(stderr, "%s: No such file or directory\n", fullPath);
    } else {
      if (newNode == NULL) {
        printDirectoryMessage(currentNode, newPath);
        return;
      }
      // adaug cheia fisierului la noua adresa
      char files[1][PATH_L];
      strcpy(files[0], param[i]);
      touch(newNode, files, 1);
      int child = -1;
      // obtin locul de un incep stergerea cheii
      Node *bifurcation = getBifurcation(currentNode, param[i], &child);
      //stergere, tratare cazuri speciale
      if (child == -1) {
        if (bifurcation->childrenNumber > 0) {
          bifurcation->asterisk = 0;
        } else {
          bifurcation = getDirectory(bifurcation);
          bifurcation->childrenNumber = 0;
          deleteTrie(bifurcation->children[0]);
        }
      } else {
        deleteTrie(bifurcation->children[child]);
      }
    }
  }
}
// Asemanatoare cu functia de mv, dar fara a mai sterge vechea cheie
void cp(Node *path, char param[][PARAM_LEN], int paramNumber) {
  char pathToFIle[PATH_L] = "";
  Node *currentNode = path, *newNode = path;
  char *newPath = param[paramNumber - 1];
  char fullPath[PATH_L];
  newNode = CD(newNode, newPath);
  for (int i = 0; i < paramNumber - 1; i++) {
    strcpy(fullPath, param[i]);
    splitFullPath(pathToFIle, param[i]);
    currentNode = CD(path, pathToFIle);
    if (currentNode == NULL) {
      printDirectoryMessage(currentNode, pathToFIle);
    }
    if (!isFile(currentNode, param[i])) {
      fprintf(stderr, "%s: No such file or directory\n", fullPath);
    } else {
      if (newNode == NULL) {
        printDirectoryMessage(currentNode, newPath);
        return;
      }
      char files[1][PATH_L];
      strcpy(files[0], param[i]);
      touch(newNode, files, 1);
    }
  }
}

void rmdir(Node *path, char param[][PARAM_LEN], int paramNumber) {
  Node *newPath;

  for (int i = 0; i < paramNumber; i++) {
    // deplasarea in director
    newPath = CD(path, param[i]);
    if (newPath == NULL) {
      printDirectoryMessage(path, param[i]);
      continue;
    }
    // tratarea cazului in care directorul nu e gol
    if (newPath->childrenNumber > 0) {
      fprintf(stderr, "%s: Directory not empty\n", param[i]);
      continue;
    }
    // salt cu un director in spate
    newPath = getDirectory(newPath->parent);
    int child = -1;
    char directory[PATH_L];
    char *p = strrchr(param[i], '/');
    strcpy(directory, p + 1);
    // cautarea locului din care sa inceapa stergerea cheii
    Node *bifurcation = getBifurcation(newPath, directory, &child);
    // tratarea caurilor de stergere
    if (child == -1) {
      newPath->childrenNumber = 0;
      deleteTrie(newPath->children[0]);

    } else {
      deleteTrie(bifurcation->children[child]);
      removeChild(bifurcation, child);
    }
  }
}
void rm(Node *path, char param[][PARAM_LEN], int paramNumber) {
  Node *newPath;
  char pathTo[PATH_L] = "", fullPath[PATH_L];
  for (int i = 0; i < paramNumber; i++) {
    strcpy(fullPath, param[i]);
    splitFullPath(pathTo, param[i]);
    // deplasare catre director/fisier
    newPath = CD(path, pathTo);
    if (newPath == NULL) {
      fprintf(stderr, "%s: No such file or directory\n", fullPath);
      return;
    }
    // verificare daca exista directorul sau fisierul respectiv
    if (CD(newPath, param[i]) == NULL && !isFile(newPath, param[i])) {
      fprintf(stderr, "%s: No such file or directory\n", fullPath);
      return;
    }
    int child = -1;
    // cautare loc de stergere si tratare cazuri
    Node *bifurcation = getBifurcation(newPath, param[i], &child);
    if (child == -1) {
      if (bifurcation->asterisk == 1) {
        if (bifurcation->childrenNumber > 0) {
          bifurcation->asterisk = 0;
        } else {
          bifurcation = getDirectory(bifurcation);
          bifurcation->childrenNumber = 0;
          deleteTrie(bifurcation->children[0]);
        }
      }

    } else {
      deleteTrie(bifurcation->children[child]);
      removeChild(bifurcation, child);
    }
  }
}
// afisare cale curenta(recursiv pana la radacina)
void pwd(Node *path) {
  if (path->parent == NULL) {
    fprintf(stdout, "/");
  } else {
    listPath(path->parent);
  }
  printf("\n");
}
// functie auxiliara pentru pwd
void listPath(Node *path) {
  if (path == NULL) {
    return;
  }
  listPath(path->parent);
  fprintf(stdout, "%c", path->info);
}
// deplasare speciala (cd .., cd .)
Node *specialCD(Node *path, char *newPath) {
  Node *currPath = path;
  char *tmp = malloc(strlen(newPath) + 1);
  while (strlen(newPath) > 0 && newPath[0] == '.') {
    unsigned int i = 0, dots = 0;
    unsigned int pathLength = strlen(newPath);
    while (i < strlen(newPath) && newPath[i] == '.') {
      dots++;
      i++;
    }
    if (dots == 1 && (newPath[i] == '/' || i == pathLength)) {
      strcpy(tmp, newPath + 2 - (i == pathLength));
      strcpy(newPath, tmp);
    } else if (dots == 2 && (newPath[i] == '/' || i == pathLength)) {
      strcpy(tmp, newPath + 3 - (i == pathLength));
      strcpy(newPath, tmp);
      if (path->parent != NULL) {
        currPath = getDirectory(currPath->parent);
      } else {
        free(tmp);
        return NULL;
      }
    }
  }
  free(tmp);
  return currPath;
}
// functia de deplasare
Node *CD(Node *path, char *newPath) {
  char *tmpPath = malloc((strlen(newPath) + 1) * sizeof(char));
  Node *currentPath = path;
  strcpy(tmpPath, newPath);
  // rezolvam cazurile speciale
  currentPath = specialCD(currentPath, tmpPath);
  if (currentPath == NULL) {
    free(tmpPath);
    return NULL;
  }
  if (strlen(tmpPath) == 0) {
    free(tmpPath);
    return currentPath;
  }
  // tratare caz radacina
  if (tmpPath[0] == '/') {
    currentPath = goToRoot(currentPath);
    memmove(tmpPath, tmpPath + 1, strlen(tmpPath));
  }
  if (strlen(tmpPath) == 0) {
    free(tmpPath);
    return currentPath;
  }
  // deplasare catre o calea
  currentPath = goToPath(currentPath, tmpPath);
  free(tmpPath);
  if (currentPath == NULL) {
    return NULL;
  }
  return currentPath;
}
// functie de afisare a fisierelor si a folderelor
void listFD(Node *path, int type, char *FDPath) {
  char FDPath_2[strlen(FDPath)];
  strcpy(FDPath_2, FDPath);
  // oprire daca intalnim caracterul '/' pentru folder sau cand se ajunge la un nod fara copii
  if (path->info == '/' || path->asterisk == 1) {
    if (type == 1) {
      char character[3];
      character[0] = path->info;
      character[1] = '\0';
      character[2] = '\0';
      // de aici pot aparea stelute false
      if (path->asterisk == 1) {
        character[1] = '*';
      }
      strcat(FDPath_2, character);
    } else if (path->asterisk == 1) {
      char character[2];
      character[0] = path->info;
      character[1] = '\0';
      strcat(FDPath_2, character);
    }
    // ne asiguram ca sunt sterse inainte de afisare
    removeAsterisk(FDPath_2);
    fprintf(stdout, "%s ", FDPath_2);
    if (path->info == '/') {
      return;
    }
  } else {
    // adaugare caracter curent la calea spre fisier/director
    char character[2];
    character[0] = path->info;

    character[1] = '\0';
    strcat(FDPath_2, character);
  }
  for (int i = 0; i < path->childrenNumber; i++) {
    listFD(path->children[i], type, FDPath_2);
  }
}
void ls(Node *path, char param[][PARAM_LEN], int paramNumber) {
  // stabilirea tipului de ls in functie de numarul de parametrii si de tipul acestora
  int type = 0;
  int index = 1;
  if (paramNumber == 0) {
    index = -1;
  }

  else if (strcmp(param[0], "-F") == 0) {
    type = 1;
  } else {
    index = 0;
  }
  if (index != -1 && index < paramNumber) {
    path = CD(path, param[index]);
  }
  char FDPath[PATH_L] = "";
  for (int i = 0; i < path->childrenNumber; i++) {
    listFD(path->children[i], type, FDPath);
  }
  fprintf(stdout, "\n");
}
