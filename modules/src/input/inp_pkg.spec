/*	This is the specification of the generic part of the input package.
	It can be instantiated by #defining or not #defining
	INP_TYPE, INP_VAR, INP_READ_IN_ONE, and INP_NPUSHBACK.
	INP_TYPE is the type of the variable INP_VAR, which contains
	all values the user wants to save when an InsertFile is done,
	and restored when an input stream is continued after a suspend.
	For instance, line numbers and position within a line might
	be interesting.
	Not defining INP_TYPE has the effect that the instantiation is
	done without saving and restoring INP_VAR.
	Defining INP_READ_IN_ONE has the effect that files will be read
	completely with one "read". Only use this if you have lots of
	memory. Not defining it causes files to be read in blocks, with
	a suitable blocksize.
	INP_NPUSHBACK is the number of PushBacks that are guaranteed
	to work. Its default value is 1.
*/

#include <ansi.h>

/* INPUT PRIMITIVES */

#define	LoadChar(dest)	((void)((dest = *_ipp++) || (dest = loadbuf())))
#define	PushBack()	(--_ipp)
#define ChPushBack(ch)	(*--_ipp = (ch))

/*	EOI may be defined as -1 in most programs but the character -1 may
	be expanded to the int -1 which causes troubles with indexing.
*/
#define	EOI	(0200)

extern char *_ipp;

_PROTOTYPE(int loadbuf, (void));
_PROTOTYPE(int AtEoIT, (void));
_PROTOTYPE(int AtEoIF, (void));
_PROTOTYPE(int InsertFile, (char *, char **, char **));
_PROTOTYPE(int InsertText, (char *, int));

/*	int InsertFile(filename, table, result)
		char *filename; 
		char **table;
		char **result;
	
	This function suspends input from the current input stream. The next
	characters are obtained from the file indicated by "filename". This file
	will be looked for in the directories, mentioned in the null-terminated
	list indicated by "table". It returns 1 if it succeeds, 0 if it fails.
	"result" will contain the full path if InsertFile returns 1.

	int InsertText(text, length)
		char *text;
		int length;
	This funtion suspends input from the current input stream. The next
	input characters are obtained from the string indicated by "text",
	whose length is indicated by "length".
	It returns 1 if it succeeds, 0 if it fails.
*/
