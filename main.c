#include "commands.h"
#include "node.h"
#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LENGTH 100
#define C_LEN 10
#define C_NUMBER 9
#define PARAM_LEN 100
// obtinerea id de comanda pentru a fi apelata functia corespunzatoare
int getCId(char *cmd) {
  char commands[C_NUMBER][C_LEN] = {"pwd",   "ls", "cd", "mkdir", "touch",
                                    "rmdir", "rm", "mv", "cp"};
  for (int i = 0; i < C_NUMBER; i++) {
    if (strcmp(cmd, commands[i]) == 0) {
      return i;
    }
  }
  return 9;
}
// obtinerea separata a parametriilor
void splitParameters(char *line, char parameters[][PARAM_LEN], int *number) {
  char *p = strtok(line, " ");
  while (p != NULL) {
    strcpy(parameters[(*number)], p);
    *number = *number + 1;
    p = strtok(NULL, " ");
  }
}
int main() {
  // Trie ul problemei
  Trie linuxSystem;
  linuxSystem.root = newNode('/', 0, NULL);
  Node *currentNode = linuxSystem.root;
  int commandsNumber;
  fscanf(stdin, "%d", &commandsNumber);
  char line[LENGTH], commandName[C_LEN], param[LENGTH][PARAM_LEN],
      command[LENGTH];
  int cmdId, paramNum;
  for (int i = 0; i <= commandsNumber; i++) {
    paramNum = 0;
    fgets(line, LENGTH, stdin);
    if (line[strlen(line) - 1] == '\n') {
      line[strlen(line) - 1] = '\0';
    }
    strcpy(command, line);
    if (strlen(line) == 0) {
      continue;
    }
    char *p = strchr(line, ' ');
    if (p != NULL) {
      strncpy(commandName, line, (p - line));
      commandName[p - line] = '\0';
      memmove(line, p + 1, strlen(line));
    } else {
      strcpy(commandName, line);
      strcpy(line, "");
    }
    cmdId = getCId(commandName);
    splitParameters(line, param, &paramNum);
    switch (cmdId) {
    case 0: {
      if (paramNum != 0) {
        fprintf(stderr, "%s: too many arguments​\n", command);
        break;
      }
      pwd(currentNode);
      break;
    }
    case 1: {
      if (paramNum > 2) {
        fprintf(stderr, "%s: too many arguments\n", command);
        break;
      }
      ls(currentNode, param, paramNum);
      break;
    }
    case 2: {
      if (paramNum != 1) {
        fprintf(stderr, "%s: too many arguments​\n", command);
        break;
      }
      Node *result = CD(currentNode, line);
      if (result != NULL) {
        currentNode = result;
      } else {
        printDirectoryMessage(currentNode, line);
      }

      break;
    }
    case 3: {
      if (paramNum < 1) {
        fprintf(stderr, "%s: missing operand\n", command);
        break;
      }
      mkdir(currentNode, param, paramNum);
      break;
    }
    case 4: {
      if (paramNum < 1) {
        fprintf(stderr, "%s: missing operand\n", command);
        break;
      }
      touch(currentNode, param, paramNum);
      break;
    }
    case 5: {
      if (paramNum < 1) {
        fprintf(stderr, "%s: missing operand\n", command);
        break;
      }
      rmdir(currentNode, param, paramNum);
      break;
    }
    case 6: {
      if (paramNum < 1) {
        fprintf(stderr, "%s: missing operand\n", command);
        break;
      }
      rm(currentNode, param, paramNum);
      break;
    }
    case 7: {
      if (paramNum < 2) {
        fprintf(stderr, "%s: missing operand\n", command);
        break;
      }
      mv(currentNode, param, paramNum);
      break;
    }
    case 8: {
      if (paramNum < 1) {
        fprintf(stderr, "%s: missing operand\n", command);
        break;
      }
      cp(currentNode, param, paramNum);
      break;
    }
    case 9: {
      fprintf(stderr, "%s: command not found\n", commandName);
      break;
    }
    }
  }
  deleteTrie(linuxSystem.root);
  return 0;
}