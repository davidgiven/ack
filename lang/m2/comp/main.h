/* S O M E   G L O B A L   V A R I A B L E S */

/* $Header$ */

extern char options[];	/* indicating which options were given */

extern int DefinitionModule;
			/* flag indicating that we are reading a definition
			   module
			*/

extern int SYSTEMModule;/* flag indicating that we are handling the SYSTEM
			   module
			*/
extern struct def *Defined;
			/* definition structure of module defined in this
			   compilation
			*/
extern char *DEFPATH[];	/* search path for DEFINITION MODULE's */
