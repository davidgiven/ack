main(argc,argv) char **argv ; {
	int child, waitchild ;
	int status ;
	child=fork() ;
	if ( child== -1 ) {
		printf("fork returned -1\n") ;
		return 1 ;
	}
	if ( child ) {
	/* The parent */
		printf("childno %d\n",child ) ;
		do {
			waitchild= wait(&status ) ;
			printf("Child %d, status 0x%x\n",waitchild,status) ;
			if ( waitchild== -1 ) {
				printf("No children\n") ;
				return 1 ;
			}
		} while ( waitchild!=child ) ;
		if ( argc<=1 && status != (8<<8) ) {
			printf("incorrect status return\n") ;
			return 2 ;
		}
	} else {
		/* The child */
		if ( argc>1 ) pause() ;
		return 8 ;
	}
	printf("fork/wait ok\n") ;
	return 0 ;
}
