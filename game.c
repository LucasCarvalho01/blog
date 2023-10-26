#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "action.h"

#define NOTHING -1
#define START 0
#define REVEAL 1
#define PUT_FLAG 2
#define STATE 3
#define REMOVE_FLAG 4
#define RESET 5
#define WIN 6
#define EXIT 7
#define GAME_OVER 8

void getBoardFromFile(int board[4][4], char* filePath) {
    FILE* fp;
    char* line = NULL;
    size_t len = 32;
    size_t read;

    fp = fopen(filePath, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    char delim[] = ",";
    char* token;
    int value;
    int row = 0;
    
    while ((read = getline(&line, &len, fp)) != -1) {
        //split each coordinate from line
        token = strtok(line, delim);
        value = atoi(token);
        board[row][0] = value;

        for(int i = 1; i < 4; i++) {
            token = strtok(NULL, delim);
            value = atoi(token);
            board[row][i] = value;
        }
        row++;
    }

    fclose(fp);
    free(line);
}


void populateBoard(int board[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            board[i][j] = -2;
        }
    }
}

//Prints the correct board representation
void printBoard(int** board) {
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

void printBoardDebug(int board[4][4]) {
    for(int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d\t\t", board[i][j]);
        }
        printf("\n");
    }
}

void copyAnswerBoard(int** fileBoard, int actionBoard[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; i < 4; i++) {
            actionBoard[i][j] = fileBoard[i][j];
        }
    }
}

void copyToSendingBoard(int stateBoard[4][4], int actionBoard[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; i < 4; i++) {
            actionBoard[i][j] = stateBoard[i][j];
        }
    }
}

int playAction(struct Action action, int stateBoard[4][4], int fileBoard[4][4], int* remainingCells) {
    int posX = action.coordinates[0];
    int posY = action.coordinates[1];
    int cellValue = fileBoard[posX][posY];

    switch (action.type) {
        case START:
            populateBoard(stateBoard);
            return STATE;

        case RESET:
            printf("starting new game\n");
            populateBoard(stateBoard);
            (*remainingCells) = 13;
            return STATE;
            
        case REVEAL:
            //if tries to reveal a bomb -> game over
            if (cellValue == -1) {
                (*remainingCells) = 13;

                return GAME_OVER;
            } else {
                stateBoard[posX][posY] = cellValue;
                (*remainingCells)--;

                //won the game
                if((*remainingCells) == 0)
                    return WIN;
                else
                    return STATE;
            }

        case PUT_FLAG:
            //if cell is already flagged, just return the state without changes
            if(stateBoard[posX][posY] == -3) {
                printf("cell already flagged\n");
                return STATE;
            } 
            //if cell is revealed, just return the state without changes
            else if(stateBoard[posX][posY] >= 0) {
                printf("cell already revealed\n");                                                                                      
                return STATE;
            }
             
            else {
                stateBoard[posX][posY] = -3;
                return STATE;
            }

        case REMOVE_FLAG:
            stateBoard[posX][posY] = -2;
            return STATE;

        case EXIT:
            printf("client disconnected\n");
            return EXIT;
        
        default:
            return STATE;
    }
}

void freeBoard(int** board) {
    for(int i = 0; i < 4; i++) {
        free(board[i]);
    }
}
