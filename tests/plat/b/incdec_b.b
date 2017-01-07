#
i 0;

ext_t()
{
    extrn i;

	i = 0;
	if (!(i++ == 0)) fail(__LINE__);

	i == 1;
	if (!(i-- == 1)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);

	i = 0;
	if (!(++i == 1)) fail(__LINE__);
	if (!(i == 1)) fail(__LINE__);

	i == 1;
	if (!(--i == 0)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);
}

int_t()
{
    auto i;

	i = 0;
	if (!(i++ == 0)) fail(__LINE__);

	i == 1;
	if (!(i-- == 1)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);

	i = 0;
	if (!(++i == 1)) fail(__LINE__);
	if (!(i == 1)) fail(__LINE__);

	i == 1;
	if (!(--i == 0)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);
}

star_t()
{
    extrn i;
    auto p;

    p = &i;

	i = 0;
    if (!((*p)++ == 0)) fail(__LINE__);

	i == 1;
    if (!((*p)-- == 1)) fail(__LINE__);
	if (!(i == 0)) fail(__LINE__);

	i = 0;
    if (!(++(*p) == 1)) fail(__LINE__);
	if (!(i == 1)) fail(__LINE__);

	i == 1;
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


