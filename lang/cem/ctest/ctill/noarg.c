char rcs_id[] = "$Header$" ;

main() {
	none() ;
	printf("Undetected: declaration of argument not present in argument list\n") ;
	return 1 ;
}

int name ;
none() int name ; { }
