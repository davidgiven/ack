/* $Header$ */
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit
 *        developed by Andrew  S.  Tanenbaum, Johan W. Stevenson,
 *      Ed Keizer and Hans van Staveren of the Vrije Universiteit.
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

exit(code)
{
	_cleanup() ;
	_exit(code) ;
}
