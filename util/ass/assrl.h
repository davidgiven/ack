/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-03-13
 *  
 */
#ifndef ASSRL_H_
#define ASSRL_H_

/* Generates an e.out file from the the temporary code file "tfile" and
 * data temporary "dfile" file. Output the data to "ifile".
 */
void copyout(void);
/* Update the relocation entries and place them into "rtfile" and "rdfile". */
void upd_reloc(void);

relc_t * data_reloc(char *arg ,FOFFSET off, int typ);
relc_t * text_reloc(glob_t *glosym, FOFFSET off, int typ);

#endif /* ASSRL_H_ */
