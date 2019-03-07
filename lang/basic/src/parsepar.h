/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef __PARSEPAR_H_INCLUDED__
#define __PARSEPAR_H_INCLUDED__

/** -l listing required */
extern int	listing;
/** -d compiler debugging */
extern int	debug;
/** -w warning flags */
extern int	wflag;
/** generate line tracing code. */
extern int	traceflag;
/** generate LIN statements */
extern int	nolins;


void parseparams(int argc,char **argv);

#endif /* __PARSEPAR_H_INCLUDED__ */
