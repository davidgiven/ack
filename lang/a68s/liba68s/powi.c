POWI(statlink, pow , num)
  int *statlink ;
  int pow , num ;
  {
    if ( pow < 0 )
      POWNEG() ;
    else
      {
        int n , p , r ;
        n = num ; p = pow ;
	if ( ( p & 1 ) != 0 ) { r = n; } else { r = 1; }
	p >>= 1 ;
        while ( p != 0 ) {
	    n *= n ;
            if ( ( p & 1 ) != 0 ) r *= n ;
            p >>= 1 ;
          }
        return( r ) ;
      }
    }
