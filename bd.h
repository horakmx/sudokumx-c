#pragma once
#include "bu.h"
#include <stdio.h>
#include <string.h>

typedef struct BoardDataS {
	unsigned char  board[POINTS];           // each: 1-9 
	unsigned short viewsPossibles[VIEWS];   // each: 1010101011 map (default = FF)
	unsigned char  viewsToCheck[VIEWS];     // each: (default = FF)
	unsigned char  viewsToCheckCount;       // default = VIEWS
	unsigned short pointPossibles[POINTS];  // each: 0101010101 map (default = FF)
	unsigned short viewsDigitsPossibles[DIGITS_OFFSETTED][VIEWS]; // each (1-9 -> 0-26): 010101010 map (default = FF)
	unsigned char  remainingPointsCount;    // default = POINTS;
	unsigned char  remainingPoints[POINTS]; // each: true
	unsigned char  digitsToCheck[DIGITS_OFFSETTED]; // each: true / false
	unsigned char  isInvalid; // default = false;
	unsigned char remainingPointsMap[POINTS];
} BoardData;

void BD_boardDataInit(BoardData *bd, const char *board);
void BD_setPoint(BoardData *bd, unsigned char pointIndex, unsigned char digit);
void BD_unsetPointPossibles(BoardData *bd, unsigned char pointIndex, unsigned short bitGroup);
int BD_isBoardSolved(BoardData *bd);
int BD_validateStartingBoard(unsigned char *board);
void BD_printBoard(BoardData *bd);
void BD_printBoardExplain(BoardData *bd, int filter);
const char *intToBin(int x);