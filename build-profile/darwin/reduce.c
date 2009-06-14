#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <assert.h>
#include <string.h>

char buffer[8192];

int off, len;

void handle_alarm ( int sig )
{
	printf("Timed out!\n");
	exit(-1);
}

int child ()
{
	/*FILE* fp;
	fp = popen("./execute", "w");
	fputs("exit", fp);
	return pclose(fp);*/
	int rc = fork(), crc;
	if (rc < 0)
	{
		printf("Fork failed.\n");
		return -1;
	}
	else if (rc == 0)
	{
		char moof[4] = {0, 0, 0, 0};
		//printf("Installing alarm handler...\n");
		signal(SIGALRM, handle_alarm);
		alarm(1);
		FILE* fp;
		//printf("Here goes.\n");
		fp = popen("./execute", "w");
		assert(fp);
		//printf("fp=%p ", fp);
		fputs("echo 'moof' > test.out", fp);
		//printf("wrote ");
		rc = pclose(fp);
		//printf("pclose=%d\n", rc);
		if (rc == 0)
		{
			FILE* fp = fopen("test.out", "r");
			if (!fp)
			{
				printf("Reported success; didn't write test.out\n");
				exit(-1);
			}
			if (fread(moof, 4, 1, fp) <= 0)
			{
				printf("Reported success; test.out was empty\n");
				fclose(fp);
				unlink("test.out");
				exit(-1);
			}
			fclose(fp);
			unlink("test.out");
			if (memcmp("moof", moof, 4))
			{
				printf("Reported success; found no dogcow\n");
				exit(-1);
			}
			printf("Permutation found @ off=%d, len=%d\n", off, len);
			exit(0);
		}
		else
		{
			unlink("test.out");
			exit(-1);
		}
	}
	else
	{
		pid_t pid = waitpid(rc, &crc, 0);
		assert(pid == rc);
		return crc;
	}
}

int main ()
{
	FILE* fp;
	int start, end, rc, ilen, bpw = sizeof(void*);
	if (child())
	{
		return 1;
	}
	printf("Right reducing...\n");
	fp = fopen("inject.profile", "r+");
	len = fread(buffer, 1, sizeof(buffer), fp);
	rewind(fp);
	end = ilen = len;
	off = 0;
	while (len > 0)
	{
		len--;
		ftruncate(fileno(fp), len);
		rc = child();
		if (rc == 0) end = len;
	}
	fclose(fp);
	printf("Trimmed %d bytes from tail\n", ilen - end);
	printf("Left reducing...\n");
	off = start = 0;
	while (off < end)
	{
		off++;
		fp = fopen("inject.profile", "w");
		fwrite(buffer + off, end - off, 1, fp);
		fclose(fp);
		rc = child();
		if (rc == 0) start = off;
	}
	printf("Trimmed %d bytes from head\n", start);
	printf("Final profile size: %d bytes (%d words)\n", end - start, (end - start + bpw - 1)/bpw);
	fp = fopen("inject.profile", "w");
	fwrite(buffer + start, end - start, 1, fp);
	fclose(fp);
	return 0;
}
