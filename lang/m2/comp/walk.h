/* P A R S E   T R E E   W A L K E R */

/*	Definition of WalkNode macro
*/

extern int (*WalkTable[])();

#define	WalkNode(xnd, xlab)	((xnd) && (*WalkTable[(xnd)->nd_class])((xnd), (xlab)))

extern label	text_label;
extern label	data_label;
