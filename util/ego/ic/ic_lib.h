/*  I N T E R M E D I A T E   C O D E
 *
 *  L I B R A R Y   M A N A G E R
 */


extern	FILE	*next_file();	/* (int argc, char *argv[])
				 * See if there are any more EM input files.
				 * 'argv' contains the names of the files
				 * that are passed as arguments to ic.
				 * If an argument is a library (archive
				 * file) only those modules that are useful
				 * are used.
				 */
