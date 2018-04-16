#include "BoardData.h"
#include "BoardUtils.h"
#include <stdio.h>

void BD_boardDataInit(BoardData *bd, const char *board) {
	bd->remainingPointsCount = POINTS;
	bd->viewsToCheckCount = VIEWS;
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
	// BD_printBoard(bd);
	// BD_printBoardExplain(bd, 0xFFFF);
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
		// if (bd->viewsPossibles[viewId] == 0) {
		//	unset bd->viewsToCheck[viewId]
		// }
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

int BD_validateStartingBoard(unsigned char *board) {

	if (strlen(board) != POINTS)
	{
		// echo "invalid size of board"
		return 0;
	}
	int setCount = 0;
	for (int i = 0; i < POINTS; i++) {
		unsigned char cel = board[i];
		if (cel == 0) continue;
		// at least 17 cels is set
		if (++setCount > 16)
		{
			return 1;
		}
	}
	// echo "INPUT ERROR: Invalid board, <= 16 set cells\n";
	return 0;
}

void BD_printBoard(BoardData *bd) {
	printf("\n");
	for (int i = 0; i < 9; i++)	{
		char extraRowClass[26] = "";
		if (!((i + 1) % 3) && i < 7) {
			sprintf(extraRowClass, "\n-------+-------+-------");
		}
		for (int d = 0; d < 9; d++) {
			int pos = i * 9 + d;
			char extraClass[3] = "";
			if (!((d + 1) % 3) && d < 7) {
				sprintf(extraClass, " |");
			}
			char piece = bd->board[pos];
			if (piece == 0)
			{
				piece = '.';
			} else {
				piece += 48;
			}
			printf(" %c%s", piece, extraClass);
		}
		printf("%s\n", extraRowClass);
	}
	printf("\n");
}

const char *intToBin(int x)
{
	static char b[33];
	b[0] = '\0';

	int z;
	for (z = 512; z > 0; z >>= 1)
	{
		strcat(b, ((x & z) == z) ? "1" : "0");
	}

	return b;
}
	
void BD_printDigits(BoardData *bd) {
	for (int d = 1; d < DIGITS_OFFSETTED; d++) {
		printf("Digit = %d\n", d);
		for (int v = 0; v < VIEWS; v++) {
			printf("view: %d = %s\n", v, intToBin(bd->viewsDigitsPossibles[d][v]));
		}
	}
}

void BD_printBoardExplain(BoardData *bd, int filter) {
	int pc = 0;
	char board[9][9][3][3];
	for (int i = 0; i < 9; i++) {
		for (int d = 0; d < 9; d++) {
			int pos = i * 9 + d;
			for (int x = 0; x < 9; x++) {
				char p = (bd->pointPossibles[pos] & BU__digits(x+1) & filter) ? (x + 1 + 48) : ' ';
				if (p != ' ')
				{
					pc++;
				}
				board[i][d][x / 3][x % 3] = p;
			}
		}
	}
	printf("\n");
	for (int i = 0; i < 9; i++) {
		for (int x = 0; x < 3; x++)
		{
			printf(" ");
			for (int d = 0; d < 9; d++) {
				char colDelim[4] = "|";
				if (!((d + 1) % 3)) {
					sprintf(colDelim, "|| ");
				}
				for (int y = 0; y < 3; y++) {
					printf("%c ", board[i][d][x][y]);
				}
				printf("%s ", colDelim);
			}
			printf("\n");
		}
		char rowDelim[40] = "-------+-------+-------++ ";
		if (!((i + 1) % 3)) {
			sprintf(rowDelim, "=======+=======+=======++ ");
		}
		printf("%s%s%s\n", rowDelim, rowDelim, rowDelim);
		if (!((i + 1) % 3))
		{
			printf("\n");
		}
	}
	printf("possibles count: %d\n", pc);
}