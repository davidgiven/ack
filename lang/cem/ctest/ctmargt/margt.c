char rcs_id[] = "$Header$" ;

main(argc,argv,envp) char **argv,**envp ; {
	register int rargc ;

	rargc=argc ;
	printf("main called with argc = %d\n",argc) ;
	printf("Arguments:\n") ;
	while ( rargc-- ) {
		printf("	%s\n",*argv++) ;
	}
	printf("Environment:\n") ;
	while ( *envp ) {
		printf("	%s\n",*envp++) ;
	}
	return(argc-1) ;
}
