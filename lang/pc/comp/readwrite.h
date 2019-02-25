/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef READWRITE_H_
#define READWRITE_H_

/* Forward structure declarations */
struct node;

struct node *ChkStdInOut(char *name, int st_out);
void ChkRead(register struct node *arg);
void ChkReadln(register struct node *arg);
void ChkWrite(register struct node *arg);
void ChkWriteln(register struct node *arg);

#endif /* READWRITE_H_ */
