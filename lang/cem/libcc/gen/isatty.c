#include <sgtty.h>

isatty(f)
{
	struct sgttyb ttyb;

	if (gtty(f, &ttyb) < 0) return 0;
	return 1;
}
