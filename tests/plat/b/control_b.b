#
zero 0;
one 1;

if_t()
{
	extrn zero, one;
	auto fails, successes;

	successes = 0;

	if (zero)
		fail(__LINE__);

	if (zero)
		fail(__LINE__);
	else
		successes++;

	if (one)
		successes++;

	if (one)
		successes++;
	else
		fail(__LINE__);

	if (successes != 3)
		fail(__LINE__);
}

while_t()
{
	extrn zero, one;
	auto successes, count;
	
	successes = 3;
	count = 0;
	while (count)
	{
		successes++;
		count--;
	}
	if (successes != 3)
		fail(__LINE__);

	while (zero)
		fail(__LINE__);

	while (one)
	{
		break;
		fail(__LINE__);
	}
}

sdata(n)
{
	switch (n)
	{
		case 0: return(0);
		case 1: return(1);
		case 100: return(100);
		default: return(-1);
	}
}

switch_t()
{
	extrn zero;
	auto successes;

	if (!(sdata(-1) == -1)) fail(__LINE__);
	if (!(sdata(0) == 0)) fail(__LINE__);
	if (!(sdata(1) == 1)) fail(__LINE__);
	if (!(sdata(2) == -1)) fail(__LINE__);
	if (!(sdata(100) == 100)) fail(__LINE__);
	if (!(sdata(200) == -1)) fail(__LINE__);

	successes = 0;
	switch (zero)
	{
		case 0: /* fall through */
		case 1: successes++; break;
	}
	if (successes != 1)
		fail(__LINE__);
}

goto_t()
{
	goto n;
	fail(__LINE__);
	n:;
}

main()
{
	if_t();
	switch_t();
	goto_t();
	while_t();

	finished();
	return(0);
}

