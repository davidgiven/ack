/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-03-12
 *  
 */
#ifndef ASSCI_H_
#define ASSCI_H_


/*
 * read module in compact EM1 code and fill in the table in memory
 * with all the specified data as a linked list.
 */
void read_compact(void);
void newline(int type);
void align(int size);

#endif /* ASSCI_H_ */
