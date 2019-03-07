/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-25
 *  
 */
#ifndef CSTOPER_H_
#define CSTOPER_H_

/* Compile time constant evaluations */


void cstunary(t_node **expp);
void cstibin(t_node **expp);
void cstfbin(t_node **expp);
void cstubin(t_node **expp);
/** Evaluates the constant set operators at compile time
 *  and returns the result in "expp".
 */
void cstset(t_node **expp);
/* Evaluates the result of internal procedures on constants
 * at compile time, and returns the result in "expp".
 */
void cstcall(t_node **expp, int call);
/* Compile time constant evaluator system initialization. */
void InitCst(void);

#endif /* CSTOPER_H_ */
