#include "f2c.h"

VOID z_sqrt(r, z)
doublecomplex *r, *z;
{
double mag, sqrt(), cabs();

if( (mag = cabs(z->r, z->i)) == 0.)
	r->r = r->i = 0.;
else if(z->r > 0)
	{
	r->r = sqrt(0.5 * (mag + z->r) );
	r->i = z->i / r->r / 2;
	}
else
	{
	r->i = sqrt(0.5 * (mag - z->r) );
	if(z->i < 0)
		z->i = - z->i;
	r->r = z->i / r->i / 2;
	}
}
