/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*   I N T E R M E D I A T E   C O D E
 *
 *    L O W   L E V E L   I / O   R O U T I N E S
 */

#include <stdio.h> /* FILE */

int	table1(void);			/*
					 * Read an instruction from the
					 * Compact Assembly Language input
					 * file (in 'neutral state').
					 */
int	table2(void);			/*
					 * Read an instruction argument.
					 */
int	table3(int);			/*
					 * Read 'Common Table' item.
					 */
short	get_int(void);
offset	get_off(void);
char	readchar(void);
void	file_init(FILE *f, short state, long length);
					/*
					 * Input file initialization. All
					 * following read operations will read
					 * from the given file f. Also checks
					 * the magic number and sets global
					 * variable 'linecount' to 0.
					 * If the state is ARCHIVE, length
					 * specifies the length of the module.
					 */
void	arch_init(FILE *arch);		/*
					 * Same as file_init,but opens an
					 * archive file. So it checks the
					 * magic number for archives.
					 */
