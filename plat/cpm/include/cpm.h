#ifndef CPM_H
#define CPM_H

#include <stdint.h>

/* EM requires 2-byte alignment, even on the i80, so we can't declare these
 * structures to contain uint16_ts. Use U16() to access them. */

typedef struct
{
    uint8_t dr;
    uint8_t f[11];
    uint8_t ex;
    uint8_t s1;
    uint8_t s2;
    uint8_t rc;
    uint8_t d[16];
    uint8_t cr;
    uint8_t r[3];
}
FCB;

typedef struct
{
    uint8_t dr;
    uint8_t src[11];
    uint8_t _padding[5];
    uint8_t dest[11];
}
RCB;

typedef struct
{
    uint8_t us;
    uint8_t f[11];
    uint8_t ex;
    uint8_t s[2];
    uint8_t rc;
    uint8_t al[16];
}
DIRE;

typedef struct
{
    uint8_t spt[2]; /* number of 128-byte sectors per track */
    uint8_t bsh;  /* block shift; 3=1kB, 4=2kB, 5=4kB etc */
    uint8_t blm;  /* block mask; 0x07=1kB, 0x0f=2kB, 0x1f=4k etc */
    uint8_t exm;  /* extent mask */
    uint8_t dsm[2]; /* maximum block number */
    uint8_t drm[2]; /* maximum directory entry number */
    uint8_t al[2];  /* directory allocation bitmap */
    uint8_t cks[2]; /* checksum vector size */
    uint8_t off[2]; /* number of reserved tracks */
}
DPB;

/* Access an unaligned field (see above). */
#define U16(ptr) (*(uint16_t*)(ptr))

extern FCB cpm_fcb; /* primary FCB */
extern FCB cpm_fcb2; /* secondary FCB (special purpose) */
extern uint8_t cpm_iobyte;

extern uint8_t cpm_default_dma[128]; /* also contains the parsed command line */
extern uint8_t* cpm_ram;
extern uint8_t* cpm_ramtop;
extern uint8_t cpm_cmdlinelen;
extern char cpm_cmdline[0x7f];

/* Special: if the CCP hasn't been overwritten, returns to it; otherwise does
 * a warmboot. */
extern void cpm_exit(void);

/* Extends cpm_ramtop over the CCP, for a little extra space. */
extern void cpm_overwrite_ccp(void);

/*  0 */ extern void cpm_warmboot(void);
/*  1 */ extern uint8_t cpm_conin(void);
/*  2 */ extern void cpm_conout(uint8_t b);
/*  3 */ extern uint8_t cpm_auxin(void);
/*  4 */ extern void cpm_auxout(uint8_t b);
/*  5 */ extern void cpm_lstout(uint8_t b);
/*  6 */ extern uint8_t cpm_conio(uint8_t b);
/*  7 */ extern uint8_t cpm_get_iobyte(void);
/*  8 */ extern void cpm_set_iobyte(uint8_t iob);
/*  9 */ extern void cpm_printstring(const char* s); /* $-terminated */
/* 10 */ extern uint8_t cpm_readline(uint8_t* buffer);
/* 11 */ extern uint8_t cpm_const(void);
/* 12 */ extern uint16_t cpm_get_version(void);
/* 13 */ extern void cpm_reset_disk_system(void);
/* 14 */ extern void cpm_select_drive(uint8_t disk);
/* 15 */ extern uint8_t cpm_open_file(FCB* fcb);
/* 16 */ extern uint8_t cpm_close_file(FCB* fcb);
/* 17 */ extern uint8_t cpm_findfirst(FCB* fcb);
/* 18 */ extern uint8_t cpm_findnext(FCB* fcb);
/* 19 */ extern uint8_t cpm_delete_file(FCB* fcb);
/* 20 */ extern uint8_t cpm_read_sequential(FCB* fcb);
/* 21 */ extern uint8_t cpm_write_sequential(FCB* fcb);
/* 22 */ extern uint8_t cpm_make_file(FCB* fcb);
/* 23 */ extern uint8_t cpm_rename_file(RCB* rcb);
/* 24 */ extern uint16_t cpm_get_login_vector(void);
/* 25 */ extern uint8_t cpm_get_current_drive(void);
/* 26 */ extern void cpm_set_dma(void* ptr);
/* 27 */ extern uint8_t* cpm_get_allocation_vector(void);
/* 28 */ extern void cpm_write_protect_drive(void);
/* 29 */ extern uint16_t cpm_get_readonly_vector(void);
/* 30 */ extern uint8_t cpm_set_file_attributes(FCB* fcb);
/* 31 */ extern DPB* cpm_get_dpb(void);
/* 32 */ extern uint8_t cpm_get_set_user(uint8_t user);
/* 33 */ extern uint8_t cpm_read_random(FCB* fcb);
/* 34 */ extern uint8_t cpm_write_random(FCB* fcb);
/* 35 */ extern void cpm_seek_to_end(FCB* fcb);
/* 36 */ extern void cpm_seek_to_seq_pos(FCB* fcb);
/* 37 */ extern uint8_t cpm_reset_drives(uint16_t drive_bitmap);
/* 40 */ extern uint8_t cpm_write_random_filled(FCB* fcb);

#endif
