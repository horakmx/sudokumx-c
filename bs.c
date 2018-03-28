#include "bs.h"
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
		// unsigned char a, b;
		// a = BU__bitCount(
		//	bd->viewsPossibles[BU__points(pointIndexBest, 0)] |
		//	bd->viewsPossibles[BU__points(pointIndexBest, 1)] |
		//	bd->viewsPossibles[BU__points(pointIndexBest, 2)]
		//);
		//b = BU__bitCount(
		//	bd->viewsPossibles[BU__points(bd->remainingPoints[i], 0)] |
		//	bd->viewsPossibles[BU__points(bd->remainingPoints[i], 1)] |
		//	bd->viewsPossibles[BU__points(bd->remainingPoints[i], 2)]
		//);
		// if (a < b) {
		//	pointIndexBest = bd->remainingPoints[i];
		// }
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
			// printf("ERROR\n");
			// BD_printBoard(bd);
			// BD_printBoardExplain(bd, 0xFFFF);
			// BD_printDigits(bd);
			return 0;
		}
		change = FindNakedSingle(bd);
		if (!change) change = FindHiddenSingle(bd);
		if (!change) change = FindLockedCandidates(bd);
		// if (!change) change = FindNakedPairs(bd);
		// if (!change) change = FindHiddenPairs(bd);
		// if (!change) change = FindNakedGroups(bd);
		// change |= (change ? 0 : FindFalseDigits(bd));
	}
	if (bd->remainingPointsCount > 0) {
		//printf("unfinished\n");
		//BD_printBoard(bd);
		//BD_printBoardExplain(bd, 0xFFFF);
		//BD_printDigits(bd);
	}
	else {
		//printf("solved\n");
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

int FindNakedPairs(BoardData *bd) {
	for (int i = 0; i < VIEWS; i++) {
		if (!(bd->viewsToCheck[i] & 2)) continue;
		bd->viewsToCheck[i] ^= 2;
		// skipping already filled views
		if (BU__bitCount(bd->viewsPossibles[i]) < 4) {
			continue;
		}
		unsigned char back[PREPARED_INT_TO_BIT_SIZE] = {0};
		unsigned char pairFound = 0;
		for (int d = 0; d < POINTS_PER_VIEW; d++) {
			unsigned short pointPossibles = bd->pointPossibles[BU__views(i, d)];
			if (pointPossibles == 0 || (d == 8 && !pairFound)) {
				continue;
			}
			if (BU__bitCount(pointPossibles) == 2) {
				pairFound = 1;
				if (back[pointPossibles]) {
					if (FilterFoundNakedGroup(bd, pointPossibles, i)) {
						return 1;
					}
					break;
				}
				else {
					back[pointPossibles] = 1;
				}
			}
		}
	}
	return 0;
}

int FilterFoundNakedGroup(BoardData *bd, unsigned short bitGroup, unsigned char viewIndex) {
	unsigned char change = 0;
	for (int i = 0; i < POINTS_PER_VIEW; i++) {
		unsigned char pointIndex = BU__views(viewIndex, i);
		if (bd->pointPossibles[pointIndex] & ~bitGroup && bd->pointPossibles[pointIndex] & bitGroup) {
			change = 1;
			BD_unsetPointPossibles(bd, pointIndex, bitGroup);
		}
	}
	return change;
}

int FindHiddenPairs(BoardData *bd) {
	for (int i = 0; i < VIEWS; i++) {
		if (!(bd->viewsToCheck[i] & 4)) continue;
		bd->viewsToCheck[i] ^= 4;
		// skipping already filled views
		if (BU__bitCount(bd->viewsPossibles[i]) < 5) {
			continue;
		}
		unsigned short back[PREPARED_INT_TO_BIT_SIZE] = { 0 };
		unsigned char pairFound = 0;
		for (int d = 1; d < DIGITS_OFFSETTED; d++) {
			unsigned short bit = BU__digits(d);
			unsigned short possibles = bd->viewsDigitsPossibles[d][i]; // 111111111
			// if point is already solved, or point is last in vector and there is still no pair found yet
			if (possibles == 0 || (d == 9 && !pairFound)) {
				continue;
			}
			if (BU__bitCount(possibles) == 2) { // 0010010000
				pairFound = 1;
				if (back[possibles]) {
					unsigned short bit1 = possibles & (possibles - 1); // 000001000
					unsigned short bit2 = possibles & ~bit1;           // 001000000
					unsigned char pos1 = BU__views(i, BU__bitPositions(bit1));
					unsigned char pos2 = BU__views(i, BU__bitPositions(bit2));
					// is it really hidden pair or naked pair?
					if (
						BU__bitCount(bd->pointPossibles[pos1]) > 2
						|| BU__bitCount(bd->pointPossibles[pos2]) > 2)
					{
						if (BU__bitCount(bd->pointPossibles[pos1]) > 2) {
							BD_unsetPointPossibles(bd, pos1, ~(bit | back[possibles]));
						}
						if (BU__bitCount(bd->pointPossibles[pos2]) > 2) {
							BD_unsetPointPossibles(bd, pos2, ~(bit | back[possibles]));
						}
						return 1;
					}
					else {
						back[possibles] = 0;
						continue;
					}
				}
				else {
					back[possibles] = bit;
				}
			}
		}
	}
	return 0;
}

int FindNakedGroups(BoardData *bd) {
	for (int i = 0; i < VIEWS; i++) {
		if (!(bd->viewsToCheck[i] & 8)) continue;
		bd->viewsToCheck[i] ^= 8;
		// skipping already filled views
		unsigned char bitCount = BU__bitCount(bd->viewsPossibles[i]);
		if (bitCount < 6) {
			continue;
		}
		unsigned short vector[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		unsigned char v = 0;
		for (int d = 0; d < POINTS_PER_VIEW; d++) {
			if (bd->pointPossibles[BU__views(i, d)] != 0) {
				vector[v++] = bd->pointPossibles[BU__views(i, d)];
			}
		}
		unsigned short group = NakedTripletExistsInVector(vector, v);
		if (group == 0 && bitCount > 6) group = NakedQuadExistsInVector(vector, v);
		if (group == 0 && bitCount > 7) group = NakedFiveExistsInVector(vector, v);
		if (group == 0 && bitCount > 8) group = NakedSixExistsInVector(vector, v);
		if (group != 0) {
			if (!FilterFoundNakedGroup(bd, group, i)) {
				printf("bug - filterFoundNakedGroup failed\n");
				return 0;
			}
			return 1;
		}
	}
	return 0;
}

/**
* naked triplet search in vector of bitmaps of possibles
* @param array $vector - vector consisting only from int (each int > 0)
* @return int - triplet (tri bits set in int, or 0 if nothing is found)
*/
unsigned short NakedTripletExistsInVector(unsigned short *vector, unsigned char vectorLength) {
	for (unsigned char i = 0; i < vectorLength - 2; i++) {
		if (BU__bitCount(vector[i]) > 3) {
			continue;
		}
		for (unsigned char j = i + 1; j < vectorLength - 1; j++) {
			unsigned short triplet = vector[i] | vector[j];
			if (BU__bitCount(triplet) != 3) {
				continue;
			}
			for (unsigned char k = j + 1; k < vectorLength; k++) {
				if (BU__bitCount(triplet | vector[k]) == 3) {
					unsigned short vectorBitsCombined = 0;
					for (unsigned char m = 0; m < vectorLength; m++) {
						if (vector[m] & triplet && vector[m] & ~triplet) {
							return triplet;
						}
						vectorBitsCombined |= vector[m];
					}
					// solving rare situation - ie.: [triplet1, triplet1, triplet1,   triplet2, something, triplet2, triplet2]
					// let's search for another triplet in vector
					// but only if there is posibility of second triplet with elimination (at least 7 different bits must be set)
					if (BU__bitCount(vectorBitsCombined) > 6) {
						i++;
						j = vectorLength;
						break;
					}
					else { // triplet found, but no elimination found
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

/**
* naked quad search in vector of bitmaps of possibles
* @param array $vector - vector consisting only from int (each int > 0)
* @return int - quad (quad bits set in int, or 0 if nothing is found)
*/
unsigned short NakedQuadExistsInVector(unsigned short *vector, unsigned char vectorLength) {
	for (unsigned char i = 0; i < vectorLength - 3; i++) {
		if (BU__bitCount(vector[i]) > 4) {
			continue;
		}
		for (unsigned char j = i + 1; j < vectorLength - 2; j++) {
			unsigned short tempBitMap = vector[i] | vector[j];
			if (BU__bitCount(tempBitMap) < 3 || BU__bitCount(tempBitMap) > 4) {
				continue;
			}
			for (unsigned char k = j + 1; k < vectorLength - 1; k++) {
				unsigned short quad = tempBitMap | vector[k];
				if (BU__bitCount(quad) != 4) {
					continue;
				}
				for (unsigned char n = k + 1; n < vectorLength; n++) {
					if (BU__bitCount(quad | vector[n]) == 4) {
						for (unsigned char m = 0; m < vectorLength; m++) {
							if (vector[m] & quad && vector[m] & ~quad)
							{
								return quad;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

/**
* naked five search in vector of bitmaps of possibles
* @param array $vector - vector consisting only from int (each int > 0)
* @return int - five (five bits set in int, or 0 if nothing is found)
*/
unsigned short NakedFiveExistsInVector(unsigned short *vector, unsigned char vectorLength) {
	for (unsigned char i = 0; i < vectorLength - 4; i++) {
		if (BU__bitCount(vector[i]) > 5) {
			continue;
		}
		for (unsigned char j = i + 1; j < vectorLength - 3; j++) {
			unsigned short tempBitMap = vector[i] | vector[j];
			if (BU__bitCount(tempBitMap) < 3 || BU__bitCount(tempBitMap) > 5) {
				continue;
			}
			for (unsigned char k = j + 1; k < vectorLength - 2; k++) {
				unsigned short tempBitMap2 = tempBitMap | vector[k];
				if (BU__bitCount(tempBitMap2) < 4 || BU__bitCount(tempBitMap2) > 5) {
					continue;
				}
				for (unsigned char n = k + 1; n < vectorLength - 1; n++) {
					unsigned short five = tempBitMap2 | vector[n];
					if (BU__bitCount(five) != 5) {
						continue;
					}
					for (unsigned char o = n + 1; o < vectorLength; o++) {
						if (BU__bitCount(five | vector[o]) == 5) {
							for (unsigned char m = 0; m < vectorLength; m++) {
								if (vector[m] & five && vector[m] & ~five) {
									return five;
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

/**
* naked six search in vector of bitmaps of possibles
* @param array $vector - vector consisting only from int (each int > 0)
* @return int - six (six bits set in int, or 0 if nothing is found)
*/
unsigned short NakedSixExistsInVector(unsigned short *vector, unsigned char vectorLength) {
	for (unsigned char i = 0; i < vectorLength - 5; i++) {
		if (BU__bitCount(vector[i]) > 6) {
			continue;
		}
		for (unsigned char j = i + 1; j < vectorLength - 4; j++) {
			unsigned short tempBitMap = vector[i] | vector[j];
			if (BU__bitCount(tempBitMap) < 3 || BU__bitCount(tempBitMap) > 6) {
				continue;
			}
			for (unsigned char k = j + 1; k < vectorLength - 3; k++) {
				unsigned short tempBitMap2 = tempBitMap | vector[k];
				if (BU__bitCount(tempBitMap2) < 4 || BU__bitCount(tempBitMap2) > 6) {
					continue;
				}
				for (unsigned char n = k + 1; n < vectorLength - 2; n++) {
					unsigned short tempBitMap3 = tempBitMap2 | vector[n];
					if (BU__bitCount(tempBitMap3) < 5 || BU__bitCount(tempBitMap3) > 6) {
						continue;
					}
					for (unsigned char o = n + 1; o < vectorLength - 1; o++) {
						unsigned short six = tempBitMap3 | vector[o];
						if (BU__bitCount(six) != 6) {
							continue;
						}
						for (unsigned char p = o + 1; p < vectorLength; p++) {
							if (BU__bitCount(six | vector[p]) == 6) {
								for (unsigned char m = 0; m < vectorLength; m++) {
									if (vector[m] & six && vector[m] & ~six) {
										return six;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}