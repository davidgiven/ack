/*		PARSER ERROR ADMINISTRATION		*/

#include	"LLlex.h"
#include	"Lpars.h"

extern char *symbol2str();

LLmessage(tk)	{
	err_occurred = 1;
	if (tk < 0)
		error("garbage at end of line");
	else if (tk)	{
		error("%s missing", symbol2str(tk));
		skipline();
		DOT = EOF;
	}
	else
		error("%s deleted", symbol2str(DOT));
}
