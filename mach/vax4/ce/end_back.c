#include <out.h>
#include "mach.h"
#include "data.h"
#include "back.h"
#include "header.h"

static do_algn();

end_back()
{
	do_algn();                 
	do_local_relocation();
	output();
}


static
do_algn()
{
	while ( ( text - text_area) % EM_WSIZE != 0 ) 
		text1( '\0');
	while ( ( data - data_area) % EM_WSIZE != 0 )
		con1( '\0');
}
