/* t o k e n . h
 *
 * Contains the structure declaration for a token
 */
struct token {
	int	t_tokno;	/* token number */
	char	t_attrib;	/* it's attribute (always a character) */
};

extern struct token dot;	/* current token */
