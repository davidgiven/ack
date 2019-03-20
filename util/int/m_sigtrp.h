/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-03-17
 *  
 */
#ifndef M_SIGTRP_H_
#define M_SIGTRP_H_

void init_signals(void);
int do_sigtrp(int tn, int sn);
void trap_signal(void);

#endif /* M_SIGTRP_H_ */
