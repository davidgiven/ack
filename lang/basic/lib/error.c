#include <stdlib.h>
#include <stdio.h>
#include "lib.h"

int _errsym;
int _erlsym;

/* error takes an error value in the range of 0-255 */
/* and generates a trap */

char* errortable[255] = {
	/* 0  */ "",
	/* 1  */ "RETURN without GOSUB",
	/* 2  */ "Out of data",
	/* 3  */ "Illegal function call",
	/* 4  */ "Overflow",
	/* 5  */ "Out of memory",
	/*  6 */ "Undefined line ",
	/*  7 */ "Subscript out of range",
	/*  8 */ "Redimensioned array",
	/*  9 */ "Division by zero",
	/* 10 */ "Illegal indirect",
	/* 11 */ "Type mismatch",
	/* 12 */ "Out of string space",
	/* 13 */ "String too long",
	/* 14 */ "String formula too complex",
	/* 15 */ "Can't continue",
	/* 16 */ "Undefined user function",
	/* 17 */ "No resume",
	/* 18 */ "Resume without error",
	/* 19 */ "Unprintable error",
	/* 20 */ "Missing operand",
	/* 21 */ "Line buffer overflow",
	/* 22 */ "FOR without NEXT",
	/* 23 */ "WHILE without WEND",
	/* 24 */ "WEND without WHILE",
	/* 25 */ "Field overflow",
	/* 26 */ "Internal error",
	/* 27 */ "Bad file number",
	/* 28 */ "File not found",
	/* 29 */ "Bad file mode",
	/* 30 */ "File already open",
	/* 31 */ "Disk IO error",
	/* 32 */ "File already exists",
	/* 33 */ "Disk full",
	/* 34 */ "Input past end",
	/* 35 */ "Bad record number",
	/* 36 */ "Bad file name",
	/* 37 */ "Direct statement in file",
	/* 38 */ "Too many files",
	/* 39 */ "File not open",
	/* 40 */ "Syntax error in data",
	0
};

void error(int index)
{
	_setline();
	if (index < 0 || index > 40)
		printf("LINE %d:ERROR %d: Unprintable error\n", _erlsym, index);
	else
		printf("LINE %d:ERROR %d: %s\n", _erlsym, index, errortable[index]);
	_errsym = index;
	_trap();
}
