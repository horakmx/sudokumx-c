#include "BoardUtils.h"  // board utils
#include "BoardData.h"   // board data
#include "BoardSolver.h" // board solver
#include "runner.h"
extern int guess;
int main(int argc, char *argv[])
{
	FILE *fd;
	unsigned long time, total, temp;
	unsigned long counts[3];
	char buffer[1024], board[82], *cptr, *dptr;

	total = 0;
	counts[0] = counts[1] = counts[2] = 0;
	if (argc>1) {
		fd = fopen(argv[1], "r");
		if (!fd) {
			printf("Unable to open '%s' for input!\n", argv[1]);
			return(1);
		}
	}
	else fd = stdin;	
	time = MilliTime();
	while (fgets(buffer, 1024, fd)) {
		dptr = board;
		for (cptr = buffer; *cptr && dptr - board<81; ++cptr) {
			if (isdigit(*cptr) || *cptr == '.') *dptr++ = *cptr;
			else if (*cptr == '#') break;
		}
		*dptr = 0;
		if (dptr - board == 81) {
			++total;
			//counts[1] += 1;
			counts[SolveBoard(board)] += 1;
		}
	}
	time = MilliTime() - time;
	fclose(fd);
	if (time == 0) time = 1;
	if (total<42949) temp = (total * 100000) / time;
	else if (total<429490) temp = (total * 10000) / ((time + 5) / 10);
	else if (total<4294900) temp = (total * 1000) / ((time + 50) / 100);
	else if (total<42949000) temp = (total * 100) / ((time + 500) / 1000);
	else if (total<429490000) temp = (total * 10) / ((time + 5000) / 10000);
	else temp = total / ((time + 50000) / 100000);
	printf("Examined %s puzzles in %lu.%03lu seconds or %s.%02lu puzzles/sec\n",
		CommaStr(total), time / 1000, time % 1000, CommaStr(temp / 100), temp % 100);
	printf("Solved: %s puzzles, %s invalid, %s multi-solution\n",
		CommaStr(counts[1]), CommaStr(counts[0]), CommaStr(counts[2]));
	printf("guesses %d\n", guess);
	return(0);
}