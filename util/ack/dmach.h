/***************************************************************/
/*                                                             */
/*   Definition for table that maps a name on an intable index */
/*                                                             */
/***************************************************************/


typedef struct {
	char *ma_name ;         /* The name of the machine */
	int   ma_index ;
} dmach ;

extern dmach massoc[] ;

extern char  intable[] ;
