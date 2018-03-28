#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if defined(_WIN32) || defined(_WINDOWS)
#include <time.h>
#else
#include <sys/resource.h>
#endif // Windows

#if defined(_WIN32) || defined(_WINDOWS)
static unsigned long
MilliTime(void)					// 1,000ths of a second of real time
{
	return (unsigned long)(clock() / (double)CLOCKS_PER_SEC * 1000);
}
#else
static unsigned long
MilliTime(void)					// 1,000ths of a second of CPU time
{
	struct rusage usage;

	if (getrusage(RUSAGE_SELF, &usage) >= 0)
		return(usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000);
	return(0);
}
#endif // Windows

static void						// Format a number with commas
PrintComma(char *dest, unsigned long value)
{
	if (value >= 1000000000) sprintf(dest, "%ld,%03ld,%03ld,%03ld",
		value / 1000000000, value / 1000000 % 1000, value / 1000 % 1000, value % 1000);
	else if (value >= 1000000) sprintf(dest, "%ld,%03ld,%03ld",
		value / 1000000, value / 1000 % 1000, value % 1000);
	else if (value >= 1000) sprintf(dest, "%ld,%03ld",
		value / 1000, value % 1000);
	else sprintf(dest, "%ld", value);
}

static const char *
CommaStr(unsigned long value)	// Format a number with commas for (s,f)printf
{
	static int indx = 0;
	static char strs[4][16];

	indx = (indx + 1) & 3;
	PrintComma(strs[indx], value);
	return(strs[indx]);
}
