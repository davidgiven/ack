/* I N S T A N T I A T I O N   O F   I N P U T   P A C K A G E */

#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    "parameters.h"
#include	"f_info.h"
struct f_info	file_info;
#include	"input.h"
#include	<em_arith.h>
#include	"idf.h"
#include	<inp_pkg.body>


AtEoIF()
{
	/*	Make the unstacking of input streams noticable to the
	   	lexical analyzer
	*/
	return 1;
}
