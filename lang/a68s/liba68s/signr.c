SIGNR(statlink, n)
  int *statlink ;
  double n ;
  { return( n < 0.0 ? - 1 : n == 0.0 ? 0 : 1 ) ; }
