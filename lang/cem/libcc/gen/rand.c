static long seed = 1L;

int rand()
{
  seed = (1103515245L * seed + 12345) & 0x7FFFFFFF;
  return((int) ((seed >> 8) & 077777));
}

srand(n)
  unsigned n;
{
  seed = n;
}
