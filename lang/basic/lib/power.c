/* $Id$ */

/*
	computes a^b.
	uses log and exp
*/

double _log(), _exp();

double
_power(base,pownr)
double pownr, base;
{
	double temp;
	long l;

	if(pownr <= 0.0) {
		if(pownr == 0.0) {
			if(base <= 0.0)
				error(3);
			return(0.0);
		}
		l = base;
		if(l != base)
			error(3);
		temp = _exp(base * _log(-pownr));
		if(l & 1)
			temp = -temp;
		return(temp);
	}
	return(_exp(base * _log(pownr)));
}
