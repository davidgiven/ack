/* $Id$ */

/* expression operators. Do not change values, as they are used as
   indices into arrays.
*/

#define E_NOT	     1
#define E_DEREF	     2
#define E_AND	     3
#define E_OR	     4
#define E_DIV	     5		/* equal to truncated quotient */
#define E_MOD	     6		/* x = (x E_DIV y) * y + x E_MOD y,
				   0 <= (x E_MOD y) < y
				*/
#define E_ZDIV	     7		/* quotient rounded to 0 */
#define E_ZMOD	     8		/* remainder of E_ZDIV */
#define E_IN	     9		/* set membership */
#define E_ARRAY	    10
#define E_PLUS	    11
#define E_MIN	    12
#define E_MUL	    13
#define E_EQUAL	    14
#define E_NOTEQUAL  15
#define E_LTEQUAL   16
#define E_GTEQUAL   17
#define E_LT	    18
#define E_GT	    19
#define E_SELECT    20
#define E_BAND	    21		/* bitwise and */
#define E_BOR	    22		/* bitwise or */
#define E_BXOR	    23
#define E_BNOT	    24
#define E_DERSELECT 25		/* -> in C */
#define E_LSFT	    26
#define E_RSFT	    27
#define E_ADDR	    28

/* long get_int(char *buf, long size, int class)
   Returns the value of size 'size', residing in 'buf', of 'class'
   T_INTEGER, T_UNSIGNED, or T_ENUM.
*/
extern long	get_int();

/* int put_int(char *buf, long size, long value)
   Stores the value 'value' of size 'size' in 'buf'.
*/
extern int	put_int();

/* double get_real(char *buf, long size)
   Returns the real value of size 'size', residing in 'buf'.
   T_INTEGER, T_UNSIGNED, or T_ENUM.
*/
extern double	get_real();

/* int put_real(char *buf, long size, double value)
   Stores the value 'value' of size 'size' in 'buf'.
*/
extern int	put_real();

/* int eval_cond(p_tree p)
   This routine evaluates the conditional expression indicated by p
   and returns 1 if it evaluates to TRUE, or 0 if it could not be
   evaluated for some reason or if it evalutes to FALSE.
   If the expression cannot be evaluated, an error message is given.
*/
extern int	eval_cond();

/* int eval_desig(p_tree p, t_addr *pbuf, long **psize, p_type *ptp)
   This routine evaluates the expression indicated by p, which should
   result in a designator. The result of the expression is an address
   which is to be found in *pbuf. *psize will contain the size of the
   designated object, and *ptp its type.
   If the expression cannot be evaluated or does not result in a
   designator, 0 is returned and an error message is given.
   Otherwise, 1 is returned.
*/
extern int	eval_desig();

/* int eval_expr(p_tree p, char **pbuf, long **psize, p_type *ptp)
   This routine evaluates the expression indicated by p.
   The result of the expression is left in *pbuf.
   *psize will contain the size of the value, and *ptp its type.
   If the expression cannot be evaluated, 0 is returned and an error
   message is given.  Otherwise, 1 is returned.
*/
extern int	eval_expr();

/* int convert(char **pbuf, long *psize, p_type *ptp, p_type tp, long size)
   This routine tries to convert the value in pbuf of size psize
   and type ptp to type tp with size size. It returns 0 if this fails,
   while producing an error message. Otherwise, it returns 1 and
   the resulting value, type and size are left in pbuf, ptp, and
   psize, respectively.
*/
extern int	convert();
