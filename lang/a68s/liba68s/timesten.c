extern	double POWR();
double TIMESTE(pow, a)
  double a;
  int pow;
  { int p ;
    double n, t, r, s;
    p = pow < 0 ? -pow : pow;
    n = 5.0;
    t = 2.0;
    if ( (p & 1) == 0 ) {r = 1.0; s = 1.0; }
    else {r = 5.0; s = 2.0; }
    p >>= 1;
    while ( p != 0 ) {
        n *= n; t *= t;
        if ( (p & 1) != 0 ) {r *= n; s *= t; }
        p >>= 1;
      }
    if (pow<0) {
      return((a/r)/s);
    } else {
      return((a*r)*s);
    }
  }
