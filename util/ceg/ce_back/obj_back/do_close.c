#include <out.h>
#include <arch.h>
#include <ranlib.h>
	
do_close()
{
	output();

	wr_close();
}
