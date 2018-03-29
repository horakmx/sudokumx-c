#include "BoardData.h"
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