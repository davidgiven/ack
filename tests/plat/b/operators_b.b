#
zero 0;
one 1;
two 2;

main()
{
	extrn zero, one, two;
	auto i;

	if (!((two + two) == 4)) fail(__LINE__);
	if (!((two - two) == 0)) fail(__LINE__);
	if (!((two * two) == 4)) fail(__LINE__);
	if (!((two / two) == 1)) fail(__LINE__);
	if (!((two % two) == 0)) fail(__LINE__);

	if (!(zero == zero)) fail(__LINE__);
	if (!(zero != one))  fail(__LINE__);
	if (!(zero <  one))  fail(__LINE__);
	if (!(zero <= one))  fail(__LINE__);
	if (!(zero <= zero)) fail(__LINE__);
	if (!(one >  zero))  fail(__LINE__);
	if (!(one >= zero))  fail(__LINE__);
	if (!(one >= one))   fail(__LINE__);

	if (!((one << 1) == 2)) fail(__LINE__);
	if (!((two >> 1) == 1)) fail(__LINE__);

	if (!((two & 1) == 0)) fail(__LINE__);
	if (!((two | 1) == 3)) fail(__LINE__);
	if (!((two ^ 2) == 0)) fail(__LINE__);

	i = 2; i =+ two; if (!(i == 4)) fail(__LINE__);
	i = 2; i =- two; if (!(i == 0)) fail(__LINE__);
	i = 2; i =* two; if (!(i == 4)) fail(__LINE__);
	i = 2; i =/ two; if (!(i == 1)) fail(__LINE__);
	i = 2; i =% two; if (!(i == 0)) fail(__LINE__);

	i = zero; i === zero; if (!i) fail(__LINE__);
	i = zero; i =!= one;  if (!i) fail(__LINE__);
	i = zero; i =<  one;  if (!i) fail(__LINE__);
	i = zero; i =<= one;  if (!i) fail(__LINE__);
	i = zero; i =<= zero; if (!i) fail(__LINE__);
	i = one;  i =>  zero; if (!i) fail(__LINE__);
	i = one;  i =>= zero; if (!i) fail(__LINE__);
	i = one;  i =>= one;  if (!i) fail(__LINE__);

	i = one; i =<< one; if (!(i == 2)) fail(__LINE__);
	i = two; i =>> one; if (!(i == 1)) fail(__LINE__);

	i = two; i =& 1; if (!(i == 0)) fail(__LINE__);
	i = two; i =| 1; if (!(i == 3)) fail(__LINE__);
	i = two; i =^ 2; if (!(i == 0)) fail(__LINE__);

	finished();
}

