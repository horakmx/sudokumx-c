#include "BoardData.h"
#include "BoardUtils.h"
#include <stdio.h>

void BD_boardDataInit(BoardData *bd, const char *board) {
	bd->remainingPointsCount = POINTS;
	bd->isInvalid = 0;
	for (int i = 0; i < VIEWS; i++) {
		bd->viewsToCheck[i] = 0xFF;
		bd->viewsPossibles[i] = 0x3FE;
	}
	for (int i = 1; i < DIGITS_OFFSETTED; i++) {
		for (int d = 0; d < VIEWS; d++) {
			bd->viewsDigitsPossibles[i][d] = 0x1FF;
		}
		bd->digitsToCheck[i] = 1;
	}
	for (int i = 0; i < POINTS; i++) {
		bd->remainingPoints[i] = i;
		bd->remainingPointsMap[i] = i;
		bd->pointPossibles[i] = 0x3FE;
		if (board[i] == '.') {
			bd->board[i] = board[i] - 46;
		}
		else {
			bd->board[i] = board[i] - 48;
		}
	}
	for (int i = 0; i < POINTS; i++) {
		if (bd->board[i] != 0) {
			BD_setPoint(bd, i, bd->board[i]);
		}
	}
}

void BD_unsetPointPossibles(BoardData *bd, unsigned char pointIndex, unsigned short bitGroup) {
	// multiple bit set?
	if ((bitGroup & (bitGroup - 1)))
	{
		// run for each single bit again
		for (int i = 1; i < DIGITS_OFFSETTED; i++) {
			unsigned short bit = BU__digits(i);
			if (bitGroup & bit) {
				BD_unsetPointPossibles(bd, pointIndex, bit);
			}
		}
		return;
	}
	// after check above -> bitGroup could not be more than one set bit
	unsigned short bit = bitGroup;
	unsigned char singleDigit = BU__bitPositions(bit);
	bd->digitsToCheck[singleDigit] = 1;
	if (bd->pointPossibles[pointIndex] & bit) {
		bd->pointPossibles[pointIndex] &= ~bit;
		for (int i = 0; i < VIEWS_PER_POINT; i++) {
			unsigned char viewIndex = BU__points(pointIndex, i);
			bd->viewsToCheck[viewIndex] = 0xFF;
			bd->viewsDigitsPossibles[singleDigit][viewIndex] &= ~BU__viewsReversed(pointIndex, i);
			if (bd->viewsDigitsPossibles[singleDigit][viewIndex] == 0 && bd->viewsPossibles[viewIndex] & bit) {
				bd->viewsPossibles[viewIndex] &= ~bit;
				bd->isInvalid = 1;
			}
		}
	}
}

void BD_setPoint(BoardData *bd, unsigned char pointIndex, unsigned char digit) {
	bd->remainingPointsCount--;
	unsigned char mapIndex = bd->remainingPointsMap[pointIndex];
	bd->remainingPoints[mapIndex] = bd->remainingPoints[bd->remainingPointsCount];
	bd->remainingPointsMap[bd->remainingPoints[bd->remainingPointsCount]] = mapIndex;
	bd->board[pointIndex] = digit;
	unsigned short bit = BU__digits(digit);
	for (int i = 0; i < VIEWS_PER_POINT; i++) {
		unsigned char viewId = BU__points(pointIndex, i);
		bd->viewsPossibles[viewId] &= ~bit;
	}
	BD_unsetPointPossibles(bd, pointIndex, bd->pointPossibles[pointIndex]);
	bd->pointPossibles[pointIndex] = 0;
	for (int i = 0; i < REF_POINTS; i++) {
		unsigned char refPointIndex = BU__refPoints(pointIndex, i);
		if (bd->pointPossibles[refPointIndex] & bit) {
			BD_unsetPointPossibles(bd, refPointIndex, bit);
		}
	}
}

int BD_isBoardSolved(BoardData *bd) {
	if (bd->remainingPointsCount != 0)
	{
		return 0;
	}
	for (int i = 0; i < POINTS; i++) {
		if (bd->board[i] == 0) {
			return 0;
		}
	}
	return 1;
}