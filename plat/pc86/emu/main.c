#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "x86emu.h"

static uint8_t ram[1025*1024];
static int floppy_fd;
static X86EMU_intrFuncs intr_funcs[256];

void printk(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

static void unknown_interrupt_cb(int num)
{
	printk("unknown interrupt 0x%02x\n", num);
	exit(1);
}

static void ignored_interrupt_cb(int num)
{
	M.x86.R_AX = 0;
}

static void int10_cb(int num)
{
	switch (M.x86.R_AH)
	{
		case 0x0e: /* print char al */
			putchar(M.x86.R_AL);
			fflush(stdout);
			break;

		default:
			printk("unknown int10 service 0x%02x\n", M.x86.R_AH);
			exit(1);
	}
}

static void int13_cb(int num)
{
	if (M.x86.R_DL != 0)
	{
		/* unknown disk */
		M.x86.R_AX = 1;
		return;
	}

	switch (M.x86.R_AH)
	{
		case 0x02: /* read sector */
		{
			/* On entry:
			 *   al: number of sectors
			 *   bx: address
			 *   ch: track
			 *   cl: sector
			 *   dh: head number
			 *   dl: disk
			 */
			uint8_t count = M.x86.R_AL;
			uint32_t address = (M.x86.R_DS << 4) + M.x86.R_BX;
			uint8_t sector = M.x86.R_CL;
			uint8_t track = M.x86.R_CH;
			uint8_t head = M.x86.R_DH;
			uint32_t lba = track*18*2 + head*18 + (sector-1);
			//printf("CHS %d.%d.%d -> lba 0x%x\n", track, head, sector, lba);
			pread(floppy_fd, &ram[address], count*512, lba*512);
			M.x86.R_AX = 0;
			CLEAR_FLAG(F_CF);
		}

		case 0x08: /* probe disk al */
			M.x86.R_AX = 0;
			M.x86.R_CL = 18; /* maximum sector number */
			M.x86.R_DH = 1; /* maximum head number */
			/* there's other stuff we should set as well */
			break;

		default:
			printk("unknown int13 service 0x%02x\n", M.x86.R_AH);
			exit(1);
	}
}

int main(int argc, const char* argv[])
{
	int i;

	if (argc != 2)
	{
		printk("syntax: pc86emu <fdimage.img>\n");
		exit(1);
	}

	floppy_fd = open(argv[1], O_RDONLY);
	if (floppy_fd == -1)
	{
		printk("could not open disk image: %s\n", strerror(errno));
		exit(1);
	}

	/* Initialise the processor. */

	M.mem_base = (uintptr_t) ram;
    M.mem_size = sizeof(ram);

	/* Load the boot sector at 0x07c0:0000. */

	for (i=0; i<512; i++)
	{
		uint8_t b;
		read(floppy_fd, &b, 1);
		wrb(0x7c00 + i, b);
	}

	/* Initialise. */

	for (i=0; i<256; i++)
		intr_funcs[i] = unknown_interrupt_cb;
	intr_funcs[0x10] = int10_cb;
	intr_funcs[0x13] = int13_cb;
	intr_funcs[0x14] = ignored_interrupt_cb; /* serial port stuff */

	//M.x86.debug = DEBUG_TRACE_F|DEBUG_STEP_F|DEBUG_DECODE_F;
	M.x86.debug = 0;
	M.x86.R_CS = 0x07c0;
	M.x86.R_IP = 0x0000;
	M.x86.R_DL = 0; /* boot drive */
	X86EMU_setupIntrFuncs(intr_funcs);
	X86EMU_exec();

	return 0;
}

