SHR(statlink, n , a)
  int *statlink ;
  unsigned a ;
  { return( n < 0 ? ( - n >= 32 ? 0 : a << - n ) : n >= 32 ? 0 : a >> n ) ; }
