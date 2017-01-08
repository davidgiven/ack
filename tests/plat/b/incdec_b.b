#
/* External variables to defeat constant folding. */
zero 0;
one 1;

i 0;

ext_t()
{
    extrn zero, one, i;

	i = zero;
	if (!(i++ == 0)) fail(__LINE__);

	i = one;
	if (!(i-- == 1)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);

	i = zero;
	if (!(++i == 1)) fail(__LINE__);
	if (!(i == 1)) fail(__LINE__);

	i = one;
	if (!(--i == 0)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);
}

int_t()
{
    extrn zero, one;
	auto i;

	i = zero;
	if (!(i++ == 0)) fail(__LINE__);

	i = one;
	if (!(i-- == 1)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);

	i = zero;
	if (!(++i == 1)) fail(__LINE__);
	if (!(i == 1)) fail(__LINE__);

	i = one;
	if (!(--i == 0)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);
}

star_t()
{
    extrn zero, one, i;
    auto p;

    p = &i;

	i = zero;
    if (!((*p)++ == 0)) fail(__LINE__);

	i = one;
    if (!((*p)-- == 1)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);

	i = zero;
    if (!(++(*p) == 1)) fail(__LINE__);
	if (!(i == 1)) fail(__LINE__);

	i = one;
    if (!(--(*p) == 0)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);
}

main()
{
    ext_t();
    int_t();
    star_t();

	finished();
	return(0);
}


