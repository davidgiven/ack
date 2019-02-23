/* T O K E N N A M E   S T R U C T U R E */

struct tokenname	{	/*	Used for defining the name of a
					token as identified by its symbol
				*/
	int tn_symbol;
	char *tn_name;
};


void reserve(register struct tokenname *resv);
