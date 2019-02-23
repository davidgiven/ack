/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef LABEL_H_
#define LABEL_H_

struct node;

void DeclLabel(struct node *nd);
void chk_labels(int Slevel);
void TstLabel(register struct node *nd, int Slevel);
void DefLabel(register struct node *nd, int Slevel);


#endif /* LABEL_H_ */
