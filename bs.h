#include "bd.h"
//s#include <malloc.h>
#include <string.h>

/**
 * Calculate number of solutions
 */
int	SolveBoard(const char *board);
char TryToSolve(BoardData *bd, unsigned char maxDepth, unsigned char depth);
unsigned char FindBestPointIndexToGuess(BoardData *bd);
int FillVisible(BoardData *bd);
int FindImpossiblePoint(BoardData *bd);
int FindNakedSingle(BoardData *bd);
int FindHiddenSingle(BoardData *bd);
int FindLockedCandidates(BoardData *bd);
int FindNakedPairs(BoardData *bd);
int FindHiddenPairs(BoardData *bd);
int FindNakedGroups(BoardData *bd);
int FilterFoundNakedGroup(BoardData *bd, unsigned short bitGroup, unsigned char viewIndex);
unsigned short NakedTripletExistsInVector(unsigned short *vector, unsigned char vectorLength);
unsigned short NakedQuadExistsInVector(unsigned short *vector, unsigned char vectorLength);
unsigned short NakedFiveExistsInVector(unsigned short *vector, unsigned char vectorLength);
unsigned short NakedSixExistsInVector(unsigned short *vector, unsigned char vectorLength);