/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-09
 *  
 */
#ifndef SKIP_H_
#define SKIP_H_

int SkipToNewLine(void);
/*	Skips skips any white space and returns the first
	non-space character.
 */
int skipspaces(register int ch, int skipnl);


#endif /* SKIP_H_ */
