/* Warning classes, at the moment three of them:
   Strict (R)
   Ordinary (W)
   Old-fashioned(O)
*/

/* Bits for a bit mask: */

#define	W_ORDINARY	1
#define W_STRICT	2
#define W_OLDFASHIONED	4

#define W_ALL		(W_ORDINARY|W_STRICT|W_OLDFASHIONED)

#define W_INITIAL	(W_ORDINARY | W_OLDFASHIONED)

/* The bit mask itself: */
extern int	warning_classes;
