/* $Header$ */

/* expression operators. Do not change values, as they are used as
   indices into arrays.
*/

#define E_NOT	1
#define E_DEREF	2
#define E_AND	3
#define E_OR	4
#define E_DIV	5		/* equal to truncated quotient */
#define E_MOD	6		/* x = (x E_DIV y) * y + x E_MOD y,
				   0 <= (x E_MOD y) < y
				*/
#define E_ZDIV	7		/* quotient rounded to 0 */
#define E_ZMOD	8		/* remainder of E_ZDIV */
#define E_IN	9		/* set membership */
#define E_ARRAY	10
#define E_PLUS	11
#define E_MIN	12
#define E_MUL	13
#define E_EQUAL	14
#define E_NOTEQUAL 15
#define E_LTEQUAL 16
#define E_GTEQUAL 17
#define E_LT	18
#define E_GT	19
#define E_SELECT 20
#define E_BAND	21		/* bitwise and */
#define E_BOR	22		/* bitwise or */
#define E_BXOR	23
#define E_BNOT	24
#define E_DERSELECT 25		/* -> in C */
#define E_LSFT	26
#define E_RSFT	27
