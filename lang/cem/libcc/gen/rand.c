/* $Id$ */
static long seed = 1L;

int rand()
{
  seed = (1103515245L * seed + 12345) & 0x7FFFFFFF;
#if _EM_WSIZE == 4
  return (int) seed;
#else
  return ((int)(seed >> 8) & 0x7FFF);
#endif
}

srand(n)
  unsigned n;
{
  seed = n;
}
