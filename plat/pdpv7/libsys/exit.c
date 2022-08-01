extern void _cleanup(void);
extern void _exit(int n);

void exit(int n)
{
	_cleanup();
	_exit(n);
}
