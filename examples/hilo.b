#

buffer[6];
PlayerName[6];

/* Taken intact from the B reference manual. */
strcopy(sl ,s2)
{
	auto i;

	i = 0;
	while (lchar(sl, i, char(s2, i)) != '*e')
		i++;
}

reads()
{
	extrn buffer;

	putstr("> ");
	flush();
	getstr(buffer);
}

atoi(s)
{
	auto value, sign, i, c;

	i = 0;
	if (char(s, i) == '-')
	{
		sign = -1;
		i++;
	}
	else
		sign = 1;

	value = 0;
	while ((c = char(s, i++)) != '*e')
		value = value*10 + (c - '0');

	return(value * sign);
}

rand()
{
	/* Genuinely random; retrieved from random.org */
	return(57);
}

game()
{
	extrn buffer;
	auto Number, Attempts;
	auto guess;

	printf("See if you can guess my number.*n");

	Number = rand() % 100;
	Attempts = 1;
	
	while (1)
	{
		reads();
		guess = atoi(buffer);
		
		if (guess == Number)
		{
			printf("*nYou got it right in only %d %s!*n", Attempts,
				(Attempts == 1) ? "go" : "goes");
			return;
		}
		
		if (guess < Number)
			printf("*nTry a bit higher.*n");
		if (guess > Number)
			printf("*nTry a bit lower.*n");
		Attempts++;
	}
}

main()
{
	extrn buffer, PlayerName;

	printf("*nHi there! I'm written in B. Before we start, what is your name?*n");
	reads();
	strcopy(PlayerName, buffer);
	printf("*nHello, %s! ", PlayerName);
	
	while (1)
	{
		game();
		printf("*nWould you like another go?*n");
		reads();
		
		if ((char(buffer, 0) == 'n') | (char(buffer, 0) == 'N'))
		{
			printf("*nThanks for playing --- goodbye!*n");
			break;
		}
		
		printf("*nExcellent! ");
	}
	
	return(0);
}

