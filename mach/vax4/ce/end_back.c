#include <out.h>
#include "mach.h"
#include "data.h"
#include "back.h"
#include "header.h"


end_back()
{
	sync();                 
	do_local_relocation();
	output();
}


sync()
{
	while ( ( text - text_area) % EM_WSIZE != 0 ) 
		text1( '\0');
	while ( ( data - data_area) % EM_WSIZE != 0 )
		con1( '\0');
}
