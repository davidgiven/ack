/* $Id$ */
/*  File   : _c2type.c
    Author : Richard A. O'Keefe.
    Updated: 23 April 1984
    Purpose: Map character codes to types

    The mapping used here is such that we can use it for converting
    numbers expressed in a variety of radices to binary as well as for
    classifying characters.
*/

char _c2type[129] =
    {	37,			/* EOF == -1 */
	37, 37, 37, 37, 37, 37, 37, 37, 37, 38, 39, 39, 39, 39, 37, 37,
	37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
	38, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
	00, 01, 02, 03, 04, 05, 06, 07,  8,  9, 36, 36, 36, 36, 36, 36,
	36, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 36, 36, 36, 36,
	36, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 36, 36, 36, 36
    };

