#include <out.h>
#include <arch.h>
#include <ranlib.h>
#include "back.h"
	
close_back()
{
	output_back();

	wr_close();
}
