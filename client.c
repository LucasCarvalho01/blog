#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "action.h"
#include "common.h"

#define START 0
#define REVEAL 1
#define PUT_FLAG 2
#define STATE 3
#define REMOVE_FLAG 4
#define RESET 5
#define WIN 6
#define EXIT 7
#define GAME_OVER 8

#define INVALID_COMMAND -1
#define INVALID_CELL -2
#define ALREADY_REVEALED -3
#define ALREADY_FLAGGED -4
#define CANNOT_FLAG -5

int takeInput(int* coordinatesArr, int boardReceived[4][4]);
int getClientAction(char *input);
void getCoordinates(int *coordinatesArr, char *coordinates);
int validateReveal(int* coordinatesArr, int boardReceived[4][4]);
int validateFlag(int* coordinatesArr, int boardReceived[4][4]);
void printBoard(int board[4][4]);
void printError(int errorCode);


int main(int argc, char **argv) {
    char* ip = argv[1];
    char* port = argv[2];
    int actionCode;
    int count;

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
    
    struct Action actionReceived;
    struct Action actionToSend;

    //listening to console commands
    while (true) {
      actionCode = takeInput(actionToSend.coordinates, actionReceived.board);
      actionToSend.type = actionCode;

      //in case of invalid command, invalid cell, invalid flag or invalid reveal
      if (actionCode < 0) {
        printError(actionCode);
        continue;
      }

      //sending message
      count = send(s, &actionToSend, sizeof(actionToSend), 0);
      if(count != sizeof(actionToSend)) {
        printf("error sending message\n");
        exit(EXIT_FAILURE);
      }

      //if exit, no need to wait for response. Disconnect and close game
      if(actionCode == EXIT) {
        break;
      }

      //receiving response
      recv(s, &actionReceived, sizeof(actionReceived), 0);

      //in case of game over or win, it's possible to reset game 
      
      if(actionReceived.type == GAME_OVER) {
        printf("GAME OVER!\n");
        printBoard(actionReceived.board);
        continue;
      } 
      if(actionReceived.type == WIN) {
        printf("YOU WIN!\n");
        printBoard(actionReceived.board);
        continue;
      }
      
      printBoard(actionReceived.board);
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
      getCoordinates(boardCoordinates, coordinates);

      // checks if some coordinate was invalid
      if (boardCoordinates[0] < 0 || boardCoordinates[0] > 3 || boardCoordinates[1] < 0 || boardCoordinates[1] > 3) {
        return INVALID_CELL;
      } else {
        coordinatesArr[0] = boardCoordinates[0];
        coordinatesArr[1] = boardCoordinates[1];
      }
    }

    intAction = getClientAction(command);

    if(intAction == REVEAL) {
      intAction = validateReveal(coordinatesArr, boardReceived);
    } else if (intAction == PUT_FLAG) {
      intAction = validateFlag(coordinatesArr, boardReceived);
    }

  return intAction;
}

int validateReveal(int* coordinatesArr, int boardReceived[4][4]) {
  int posX = coordinatesArr[0];
  int posY = coordinatesArr[1];

  // if cell is already revealed
  if (boardReceived[posX][posY] > -2) {
    return ALREADY_REVEALED;
  }

  return REVEAL;
}

int validateFlag(int* coordinatesArr, int boardReceived[4][4]) {
  int posX = coordinatesArr[0];
  int posY = coordinatesArr[1];

  // if cell is already flagged
  if (boardReceived[posX][posY] == -3) {
    return ALREADY_FLAGGED;
  }

  // if cell is already revealed
  if (boardReceived[posX][posY] >= 0) {
    return CANNOT_FLAG;
  }

  return PUT_FLAG;
}

void getCoordinates(int *coordinatesArr, char *coordinates) {
  char *token, posX, posY;
  char delim[] = ",";

  token = strtok(coordinates, delim);
  posX = atoi(token);
  coordinatesArr[0] = posX;

  token = strtok(NULL, delim);
  posY = atoi(token);
  coordinatesArr[1] = posY;
}

// Get string action and convert to specified int
int getClientAction(char *input) {
  if (!strncmp(input, "start", 5))
    return 0;
  if (!strncmp(input, "reveal", 6))
    return 1;
  if (!strncmp(input, "flag", 4))
    return 2;
  if (!strncmp(input, "remove_flag", 11))
    return 4;
  if (!strncmp(input, "reset", 5))
    return 5;
  if (!strncmp(input, "exit", 4))
    return 7;
  else
    return INVALID_COMMAND;
}

//Prints the correct board representation
void printBoard(int board[4][4]) {
    for(int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            switch (board[i][j]) {
                case -2:
                    printf("-\t\t");
                    break;

                case -1:
                    printf("*\t\t");
                    break;

                case 0:
                    printf("0\t\t");
                    break;
                
                case -3:
                    printf(">\t\t");
                    break;
                
                default:
                    printf("%d\t\t", board[i][j]);
                    break;
            }
        }
        printf("\n");
    }
}

void printError(int errorCode) {
  char error[64] = "";

  switch (errorCode) {
    case INVALID_COMMAND:
        strcpy(error, "error: command not found");
        break;

    case INVALID_CELL:
        strcpy(error, "error: invalid cell");
        break;

    case ALREADY_REVEALED:
        strcpy(error, "error: cell already revealed");
        break;

    case ALREADY_FLAGGED:
        strcpy(error, "error: cell already has a flag");
        break;

    case CANNOT_FLAG:
        strcpy(error, "error: cannot insert flag in revealed cell");
        break;

    default:
        break;
  }

  printf("%s\n", error);
}