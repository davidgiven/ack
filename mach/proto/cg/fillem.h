/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-04-14
 *  
 */
#ifndef FILLEM_H_
#define FILLEM_H_

/** Switch to text segment. */
void swtxt(void);


void in_init(char *filename);
void in_start(void);
void in_finish(void);
void fillemlines(void);

void dopseudo(void);

#endif /* FILLEM_H_ */
