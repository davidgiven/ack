/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* $Header$ */

#include <math.h>
#include <errno.h>

extern int errno;

static double
asin_acos(x, cosfl)
	double x;
{
	int	negative = x < 0;
	int	i;
	double	g;
	extern double	sqrt();
	static double p[] = {
		-0.27368494524164255994e+2,
		 0.57208227877891731407e+2,
		-0.39688862997540877339e+2,
		 0.10152522233806463645e+2,
		-0.69674573447350646411e+0
	};
	static double q[] = {
		-0.16421096714498560795e+3,
		 0.41714430248260412556e+3,
		-0.38186303361750149284e+3,
		 0.15095270841030604719e+3,
		-0.23823859153670238830e+2,
		 1.0
	};

	if (negative) {
		x = -x;
	}
	if (x > 0.5) {
		i = 1 - cosfl;
		if (x > 1) {
			errno = EDOM;
			return 0;
		}
		g = 0.5 - 0.5 * y;
		y = - sqrt(g);
		y += y;
	}
	else {
		/* ??? avoid underflow ??? */
		g = y * y;
	}
	y += y * g * POLYNOM4(g, x) / POLYNOM5(g, y);
	if (i == 1) {
		if (cosfl == 0 || ! negative) {
			y = (y + M_PI_4) + M_PI_4;
		}
		else if (cosfl && negative) {
			y = (y + M_PI_2) + M_PI_2;
		}
	}
	if (! cosfl && negative) y = -y;
	return y;
}

double
asin(x)
	double x;
{
	return asin_acos(x, 0);
}

double
acos(x)
	double x;
{
	return asin_acos(x, 1);
}
