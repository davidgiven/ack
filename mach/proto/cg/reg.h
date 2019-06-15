/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-04-14
 *  
 */
#ifndef REG_H_
#define REG_H_

void chrefcount(int regno, int amount, int tflag);
int getrefcount(int regno, int tflag);
void erasereg(int regno);
void cleanregs(void);
void chkregs(void);

#endif /* REG_H_ */
