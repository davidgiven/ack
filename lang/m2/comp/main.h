/* S O M E   G L O B A L   V A R I A B L E S */

/* $Header$ */

extern char options[];	/* Indicating which options were given */

extern int DefinitionModule;
			/* Flag indicating that we are reading a definition
			   module
			*/

extern int SYSTEMModule;/* Flag indicating that we are handling the SYSTEM
			   module
			*/
extern struct def *Defined;
			/* Definition structure of module defined in this
			   compilation
			*/
