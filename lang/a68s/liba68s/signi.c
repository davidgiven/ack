SIGNI(statlink, n)
  int *statlink ;
  int n ;
  { return( n < 0 ? - 1 : n == 0 ? 0 : 1 ) ; }
