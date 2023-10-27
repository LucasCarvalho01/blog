#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "blogoperation.h"
#include "common.h"

#define NEW_CONNECTION 1
#define NEW_POST 2
#define LIST_TOPICS 3
#define SUBSCRIBE_TOPIC 4
#define DISCONNECT 5
#define UNSUBSCRIBE_TOPIC 6

#define INVALID_COMMAND -1



int getClientAction(char *input);
void printError(int errorCode);

int main(int argc, char **argv) {
    char* ip = argv[1];
    char* port = argv[2];
    int actionCode;
    int count;

    struct BlogOperation operation;

    struct sockaddr_storage storage;
    addrparse(ip, port, &storage);

    struct sockaddr *addr = (struct sockaddr *) &storage;

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1) {
      exit(EXIT_FAILURE);
    }

    if(0 != connect(s, addr, sizeof(storage))) {
      exit(EXIT_FAILURE);
    }

    //client connected to server

    //listening to console commands
    while (true) {


      //in case of invalid command
      if (actionCode < 0) {
        printError(actionCode);
        continue;
      }

      //sending message
      count = send(s, &operation, sizeof(operation), 0);
      if(count != sizeof(operation)) {
        printf("error sending message\n");
        exit(EXIT_FAILURE);
      }

    }

    close(s);
    return 0;
}


int takeInput(int* coordinatesArr, int boardReceived[4][4]) {
  char input[64], command[64], coordinates[8];
  char *token;
  char delim[] = " ";
  int intAction;
  int boardCoordinates[2] = {-1, -1};

    // Read command
    fgets(input, 64, stdin);

    token = strtok(input, delim);
    strcpy(command, token);

    // If it passes more information, stores it
    token = strtok(NULL, delim);
    if (token != NULL) {
      strcpy(coordinates, token);

    }

    intAction = getClientAction(command);

  return intAction;
}


// Get string action and convert to specified int
int getClientAction(char *input) {
  if (!strncmp(input, "publish in", 10))
    return NEW_POST;
  if (!strncmp(input, "subscribe", 9))
    return SUBSCRIBE_TOPIC;
  if (!strncmp(input, "unsubscribe", 11))
    return UNSUBSCRIBE_TOPIC;
  if (!strncmp(input, "list topics", 11))
    return LIST_TOPICS;
  if (!strncmp(input, "exit", 4))
    return DISCONNECT;
  else
    return INVALID_COMMAND;
}


void printError(int errorCode) {
  char error[64] = "";

  switch (errorCode) {
    case INVALID_COMMAND:
        strcpy(error, "error: command not found");
        break;


    default:
        break;
  }

  printf("%s\n", error);
}