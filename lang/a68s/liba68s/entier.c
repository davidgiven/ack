int ENTIER(statlink, a)
  int *statlink ;
  register double a ;
  {
    int n ;
    n = ( int ) a ;
    return( n < 0 && ( double ) n != a ? n - 1 : n ) ;
  }
int ROUN(statlink,a)
  int *statlink ;
  register double a ;
  { return(ENTIER(statlink, a+0.5)) ;
  }
