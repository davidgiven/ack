/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-03-16
 *  
 */
#ifndef IO_H_
#define IO_H_

extern void fatal(char *fmt, ...);
extern void message(char *fmt, ...);
extern void init_ofiles(int firsttime);
extern void close_down(int rc);
extern void incr_mess_id(void);

#endif /* IO_H_ */
