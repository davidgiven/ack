main() {
	register int l,j ;

	for ( j=0 ; (l=getchar()) != -1 ; j++ ) {
		if ( j%16 == 15 ) printf("%3d\n",l&0377 ) ;
		else              printf("%3d ",l&0377 ) ;
	}
	printf("\n") ;
}
