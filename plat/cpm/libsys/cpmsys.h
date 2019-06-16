#ifndef CPMSYS_H
#define CPMSYS_H

/* File descriptor emulation */

struct FCBE
{
    uint16_t length; /* number of records */
    FCB fcb; /* drive 0 means the console, f[0] == 0 means unallocated */
    uint8_t user;
    uint8_t offset; /* into current sector */
};

#define NUM_FILE_DESCRIPTORS 8
extern struct FCBE __fd[NUM_FILE_DESCRIPTORS];
extern uint8_t __transfer_buffer[128];

extern void __init_file_descriptors(void);

#define SECTOR_ALIGNED(s) (((s) & 0x7f) == 0)

#endif
