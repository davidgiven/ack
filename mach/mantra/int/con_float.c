/* unfortunately, we cannot just do an atof and convert result to float
   if neccessary, because that might result in over/underflow at an
   unexpected moment. For the time being, overflow in floating point
   constants goes undetected
*/

#include <con_float>

con_float(str, argval, res)
	char *str, *res;
{
	float_cst(str, argval, res);
}
