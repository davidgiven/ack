#include <math.h>

double MAXR(staticlink)
  int *staticlink;
#ifdef MAXFLOAT
  { return(MAXFLOAT); }
#else
#ifdef HUGE
  { return(HUGE); }
#else
  { return(0.0); /* obviously wrong*/ }
#endif
#endif

