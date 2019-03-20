/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-03-16
 *  
 */
#ifndef IO_H_
#define IO_H_

void fatal(char *fmt, ...);
void message(char *fmt, ...);
void init_ofiles(int firsttime);
void close_down(int rc);

#endif /* IO_H_ */
