/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
char rcs_id[] = "$Id$" ;

main() {
	none() ;
	printf("Undetected: declaration of argument not present in argument list\n") ;
	return 1 ;
}

int name ;
none() int name ; { }
