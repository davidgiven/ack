/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * tunable.h $Header$
 * Tunable constants
 */

# define NNONTERMS	150	/* size	of nonterminal array */
# define NTERMINALS	150	/* size	of terminal array */
# define NAMESZ		3000	/* size	of name	table */
# define LTEXTSZ	51	/* size	of token */
# define ENTSIZ		900	/* size	of entry table, max 8191 */
