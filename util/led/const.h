/* $Header$ */

typedef int		bool;

#define FALSE		0
#define TRUE		1

#define S_ZER		0x2000		/* Internal use only.		*/

#define WIDTH		8		/* Number of bits in a byte.	*/
#define BYTEMASK	0xFF		/* Mask to get low order byte.	*/
#define MININT		(1 << (sizeof(int) * WIDTH - 1))
#define MAXCHUNK	(-(MININT + 1))	/* Highest count we write(2).	*/

#define RFLAG		0x01		/* -r flag given.		*/
#define SFLAG		0x02		/* -s flag given.		*/

#define MAXSECT		64		/* Maximum number of sections.	*/

#define PLAIN		0		/* Input file is a normal file.	*/
#define ARCHIVE		1		/* Input file is an archive.	*/

#define FIRST		1		/* Pass number.			*/
#define SECOND		2		/* Idem.			*/

#define BADOFF		((ind_t)-1)
