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

typedef struct
{
    uint8_t xlt[2];     /* address of translation vector */
    uint8_t scratch[6]; /* BDOS scratchpad */
    uint8_t dirbuf[2];  /* address of directory scratchpad */
    uint8_t dpb[2];     /* address of DPB */
    uint8_t csv[2];     /* address of disk change scratchpad */
    uint8_t alv[2];     /* address of allocation bitmap */
}
DPH;

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

/* Special: parses a filename into an FCB. Returns the user code (if any).
 * Warning: cannot fail (because CP/M filespecs are incredibly lax). */

extern uint8_t cpm_parse_filename(FCB* fcb, const char* filename);

/* Special: if the CCP hasn't been overwritten, returns to it; otherwise does
 * a warmboot. */
extern void cpm_exit(void);

/* Special: equivalent to cpm_read_random() except if you read unwritten data
 * 0 is returned (and the buffer contains garbage). */
extern uint8_t cpm_read_random_safe(FCB* fcb);

/* Extends cpm_ramtop over the CCP, for a little extra space. */
extern void cpm_overwrite_ccp(void);

/* Like cpm_printstring, but uses C-style strings terminated with a \0. */
extern void cpm_printstring0(const char* s);

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

#define cpm_get_user() cpm_get_set_user(0xff)
#define cpm_set_user(u) cpm_get_set_user(u)

extern void cpm_bios_boot(void);
extern void cpm_bios_wboot(void);
extern uint8_t cpm_bios_const(void);
extern uint8_t cpm_bios_conin(void);
extern void cpm_bios_conout(uint8_t c);
extern void cpm_bios_list(uint8_t c);
extern void cpm_bios_punch(uint8_t c);
extern uint8_t cpm_bios_reader(void);
extern void cpm_bios_home(void);
extern DPH* cpm_bios_seldsk(uint8_t disk);
extern void cpm_bios_settrk(uint16_t track);
extern void cpm_bios_setsec(uint16_t sector);
extern void cpm_bios_setdma(void* dma);
extern uint8_t cpm_bios_read(void);
extern uint8_t cpm_bios_write(void);
extern uint8_t cpm_bios_listst(void);
extern uint16_t cpm_bios_sectran(uint8_t* table, uint16_t sector);

#endif
