#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <z80ex/z80ex.h>

extern Z80EX_CONTEXT* z80;
extern uint8_t ram[0x10000];

extern void emulator_init(void);
extern void emulator_run(void);
extern void showregs(void);

extern const uint8_t ccp_data[];
extern const int ccp_len;

extern const uint8_t bdos_data[];
extern const int bdos_len;

extern void bios_coldboot(void);

extern void biosbdos_entry(int syscall);

typedef struct
{
	uint8_t drive;
	char bytes[11];
}
cpm_filename_t;

extern void files_init(void);
extern void file_set_drive(int drive, const char* path);
extern struct file* file_open(cpm_filename_t* filename);
extern struct file* file_create(cpm_filename_t* filename);
extern int file_close(cpm_filename_t* filename);
extern int file_read(struct file* file, uint8_t* data, uint16_t record);
extern int file_write(struct file* file, uint8_t* data, uint16_t record);
extern int file_getrecordcount(struct file* f);
extern void file_setrecordcount(struct file* f, int count);
extern int file_findfirst(cpm_filename_t* pattern);
extern int file_findnext(cpm_filename_t* result);
extern int file_delete(cpm_filename_t* pattern);
extern int file_rename(cpm_filename_t* src, cpm_filename_t* dest);

extern void fatal(const char* message, ...);

extern bool flag_enter_debugger;
extern char* const* user_command_line;

#endif

