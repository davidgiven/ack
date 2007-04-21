/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char buffer[32];
char PlayerName[32];

int Number;
int Attempts;

void reads(void)
{
	char* p;
	
	printf("> ");
	fflush(stdout);
	
	fgets(buffer, sizeof(buffer), stdin);
	
	p = strchr(buffer, '\n');
	if (p != NULL)
		*p = '\0';
}

void game(void)
{
	printf("See if you can guess my number.\n");

	Number = rand() % 100;
	Attempts = 1;
	
	for (;;)
	{
		int guess;
		
		printf("\n");
		reads();
		guess = atoi(buffer);
		
		if (guess == Number)
		{
			printf("\nYou got it right in only %d %s!\n", Attempts,
				(Attempts == 1) ? "go" : "goes");
			return;
		}
		
		if (guess < Number)
			printf("\nTry a bit higher.\n");
		if (guess > Number)
			printf("\nTry a bit lower.\n");
		Attempts++;
	}
}

int main(int argc, char* argv[])
{
	printf("\nHi there! I'm written in C. Before we start, what is your name?\n");
	reads();
	strcpy(PlayerName, buffer);
	printf("\nHello, %s! ", PlayerName);
	
	for (;;)
	{
		game();
		printf("\nWould you like another go?\n");
		reads();
		
		if ((buffer[0] == 'n') || (buffer[0] == 'N'))
		{
			printf("\nThanks for playing --- goodbye!\n");
			exit(0);
		}
		
		printf("\nExcellent! ");
	}
	
	return 0;
}
