/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* A program to move the file pem??.m to pem.m */
/* Called when "apc pem.p" fails. It is assumed that the binary
   file is incorrect in that case and has to be created from the compact
   code file.
   This program selects the correct compact code file for each combination
   of word and pointer size.
   It will return an error code if the move failed
*/
main(argc) {
	char copy[100] ;

	if ( argc!=1 ) {
		printf("No arguments allowed\n") ;
		exit(1) ;
	}

	sprintf(copy,"cp pem%d%d.m pem.m", EM_WSIZE, EM_PSIZE) ;
	printf("%s\n",copy) ;
	exit(system(copy)) ;
}
