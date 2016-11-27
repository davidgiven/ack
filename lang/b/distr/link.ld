OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

ENTRY(start)

SECTIONS
{
	. = 0x400000;
	.text : { *(.text) }
	. = 0x8000000;
	.data : {
		*(.data)
		__bsymb = .;
		*(.bsymb)
		__ebsymb = .;
	}
	. = ALIGN(16);
	__bss = .;
	.bss : { *(.bss) }
	__ebss = .;
}
