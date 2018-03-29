#include "BoardSolver.h"
#include <stdio.h>
#include <stdlib.h>

int guess = 0;
/**
 * Calculate number of solutions
 */
int	SolveBoard(const char *board) {
	BoardData bd;
	BD_boardDataInit(&bd, board);
	if (TryToSolve(&bd, 81, 0)) {
		return 1;
	}
	return 0;
}

char TryToSolve(BoardData *bd, unsigned char maxDepth, unsigned char depth) {
	unsigned char ret = FillVisible(bd);
	if (!ret) {
		return -1;
	}
	if (BD_isBoardSolved(bd)) {
		return 1;
	}
	if (maxDepth <= depth) {
		return -1;
	}
	BoardData *bdBack = malloc(sizeof(BoardData));
	memcpy(bdBack, bd, sizeof *bdBack);
	unsigned char pointIndex = FindBestPointIndexToGuess(bd);
	for (int d = 1; d < DIGITS_OFFSETTED; d++) {
		unsigned short bit = BU__digits(d);
		if (!(bd->pointPossibles[pointIndex] & bit)) {
			continue;
		}
		guess++;
		BD_setPoint(bd, pointIndex, d);
		char out = TryToSolve(bd, maxDepth, depth + 1);
		if (out < 1) {
			memcpy(bd, bdBack, sizeof *bd);
		}
		if (out == 1)
		{
			return 1;
		}
	}
	return 0;
}

unsigned char FindSimilarPairCount(BoardData *bd, unsigned char pointIndex) {
	unsigned char count = 0;
	for (unsigned char i = 0; i < REF_POINTS; i++) {
		if (
			BU__bitCount(bd->pointPossibles[BU__refPoints(pointIndex, i)]) == 2 &&
			bd->pointPossibles[BU__refPoints(pointIndex, i)] & bd->pointPossibles[pointIndex]) {
			count++;
		}
	}
	return count;
}

unsigned char FindBestPointIndexToGuess(BoardData *bd) {
	unsigned char pointIndexBest = 0;
	unsigned char pointPossiblesMin = POINTS;
	unsigned char bestHits = 0;
	for (unsigned char i = 0; i < bd->remainingPointsCount; i++) {
		if (BU__bitCount(bd->pointPossibles[bd->remainingPoints[i]]) < pointPossiblesMin) {
			pointPossiblesMin = BU__bitCount(bd->pointPossibles[bd->remainingPoints[i]]);
			pointIndexBest = bd->remainingPoints[i];
			bestHits = FindSimilarPairCount(bd, bd->remainingPoints[i]);
			continue;
		}
		if (BU__bitCount(bd->pointPossibles[bd->remainingPoints[i]]) > pointPossiblesMin) {
			continue;
		}
		unsigned char a = FindSimilarPairCount(bd, bd->remainingPoints[i]);
		if (a > bestHits) {
			pointIndexBest = bd->remainingPoints[i];
			bestHits = a;
		}
	}
	return pointIndexBest;
}

int FillVisible(BoardData *bd) {
	int change = 1;
	while (change) {
		change = 0;
		if (bd->remainingPointsCount == 0) {
			break;
		}
		if (bd->isInvalid || FindImpossiblePoint(bd)) {
			return 0;
		}
		change = FindNakedSingle(bd);
		if (!change) change = FindHiddenSingle(bd);
		if (!change) change = FindLockedCandidates(bd);
	}
	return 1;
}

int FindImpossiblePoint(BoardData *bd) {
	for (unsigned char i = 0; i < bd->remainingPointsCount; i++) {
		unsigned char pointIndex = bd->remainingPoints[i];
		if (bd->board[pointIndex] == 0 && bd->pointPossibles[pointIndex] == 0) {
			bd->isInvalid = 1;
			return 1;
		}
	}
	return 0;
}

int FindNakedSingle(BoardData *bd) {
	for (unsigned char i = 0; i < bd->remainingPointsCount; i++) {
		unsigned char pointIndex = bd->remainingPoints[i];
		unsigned short bitGroup = bd->pointPossibles[pointIndex];
		if (!(bitGroup & (bitGroup - 1))) // $digits is power of 2 (only one bit is set)
		{
			BD_setPoint(bd, pointIndex, BU__bitPositions(bitGroup));
			return 1;
		}
	}
	return 0;
}

int FindHiddenSingle(BoardData *bd) {
	for (int i = 0; i < VIEWS; i++) {
		if (!(bd->viewsToCheck[i] & 1)) continue;
		bd->viewsToCheck[i] ^= 1;
		for (int d = 1; d < DIGITS_OFFSETTED; d++) {
			unsigned short x = bd->viewsDigitsPossibles[d][i];
			if (x != 0 && !(x & (x - 1))) {
				BD_setPoint(bd, BU__views(i, BU__bitPositions(x)), d);
				return 1;
			}
		}
	}
	return 0;
}

int FindLockedCandidates(BoardData *bd) {
	for (int i = 0; i < LOCKED_CANDIDATES; i++) {
		unsigned short a, b, c, d[2];
		a = bd->pointPossibles[BU__lockedCandidates(i, 0)] |
			bd->pointPossibles[BU__lockedCandidates(i, 1)] |
			bd->pointPossibles[BU__lockedCandidates(i, 2)];
		b = bd->pointPossibles[BU__lockedCandidates(i, 3)] |
			bd->pointPossibles[BU__lockedCandidates(i, 4)] |
			bd->pointPossibles[BU__lockedCandidates(i, 5)] |
			bd->pointPossibles[BU__lockedCandidates(i, 6)] |
			bd->pointPossibles[BU__lockedCandidates(i, 7)] |
			bd->pointPossibles[BU__lockedCandidates(i, 8)];
		c = bd->pointPossibles[BU__lockedCandidates(i, 9)] |
			bd->pointPossibles[BU__lockedCandidates(i, 10)] |
			bd->pointPossibles[BU__lockedCandidates(i, 11)] |
			bd->pointPossibles[BU__lockedCandidates(i, 12)] |
			bd->pointPossibles[BU__lockedCandidates(i, 13)] |
			bd->pointPossibles[BU__lockedCandidates(i, 14)];
		d[0] = 0;
		d[1] = 0;
		if (a & ~b & c) { // eliminate group c
			d[0] = a & ~b;
		}
		if (a & ~c & b) { // eliminate group b
			d[1] = a & ~c;
		}
		unsigned char hit = 0;
		if (d[0] != 0) {
			for (int x = 9; x <= 14; x++) {
				if (d[0] & bd->pointPossibles[BU__lockedCandidates(i, x)]) {
					BD_unsetPointPossibles(bd, BU__lockedCandidates(i, x), d[0]);
				}
			}
			hit = 1;
		}
		if (d[1] != 0) {
			for (int x = 3; x <= 8; x++) {
				if (d[1] & bd->pointPossibles[BU__lockedCandidates(i, x)]) {
					BD_unsetPointPossibles(bd, BU__lockedCandidates(i, x), d[1]);
				}
			}
			hit = 1;
		}
		if (hit) {
			return 1;
		}
	}
	return 0;
}