double POWR(statlink, pow, num)
  int *statlink ;
  int pow ;
  double num ;
  {
    int p ;
    register double n, r;
    if (pow < 0) {
      p = -pow;
      n = 1.0/num;
    } else {
      p = pow;
      n = num;
    }
    if ( (p & 1) != 0 ) { r = n; } else { r = 1.0; }
    p >>= 1;
    while ( p != 0 ) {
        n *= n;
        if ( (p & 1) != 0 ) r *= n;
        p >>= 1;
      }
    return(r) ;
  }
