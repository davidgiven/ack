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
void ChkRead(struct node *arg);
void ChkReadln(struct node *arg);
void ChkWrite(struct node *arg);
void ChkWriteln(struct node *arg);

#endif /* READWRITE_H_ */
