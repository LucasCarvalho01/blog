#ifndef GAME_H
#define GAME_H

int** generateBoard();
int** getBoardFromFile(int board[4][4], char* filePath);
void freeBoard(int** board);
int playAction(struct Action action, int stateBoard[4][4], int fileBoard[4][4], int* remainingCells);
void copyAnswerBoard(int** fileBoard, int** actionBoard);
void copyToSendingBoard(int stateBoard[4][4], int actionBoard[4][4]);
void populateBoard(int** board);
void printBoard(int** board);
void printBoardDebug(int board[4][4]);

#endif