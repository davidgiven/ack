MOD(statlink, b , a)
  int *statlink ;
  int a , b ;
  {
    int r ;
    r = a % b ;
    return( r < 0 ? r + ( b < 0 ? - b : b ) : r ) ;
  }
