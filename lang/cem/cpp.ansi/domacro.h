/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-09
 *  
 */
#ifndef DOMACRO_H_
#define DOMACRO_H_

struct idf;

void macro_def(register struct idf* id, char* text, int nformals, int length, int flags);
void do_undef(char* argstr);
/*	Control line interpreter. The '#' has already
    been read by the lexical analyzer by which this function is called.
    The token appearing directly after the '#' is obtained by calling
    the basic lexical analyzing function GetToken() and is interpreted
    to perform the action belonging to that token.
    An error message is produced when the token is not recognized.
    Pragma's are handled by do_pragma(). They are passed on to the
    compiler.
*/
void domacro(void);
char* GetIdentifier(int skiponerr);

#endif /* DOMACRO_H_ */
