#include <stdio.h>

char buffer[8192];

typedef void (*fn)(void);

int main ()
{
	fn subroutine;
	FILE* fp = fopen("inject.profile", "r");
	fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);
	subroutine = (fn)buffer;
	subroutine();
	printf("Execution fell through.\n");
	return 1;
}
