#include <em.h>
#include <system.h>

#define codefile	B_codefile
#define cur_seg		B_cur_seg
#define saved		B_saved
#define labeltje	B_labeltje
#define name		B_name
#define output_back	B_output_back

/* Macros to change interface names */

#define common		B_common
#define con1		B_con1
#define con2		B_con2
#define con4		B_con4
#define rom1		B_rom1
#define rom2		B_rom2
#define rom4		B_rom4
#define text1		B_txt1
#define text2		B_txt2
#define text4		B_txt4
#define close_back	B_close_back
#define open_back	B_open_back
#define end_back	B_end_back
#define init_back	B_init_back
#define extnd_pro	B_ex_pro
#define extnd_start	B_ex_start
#define extnd_name	B_ex_name
#define extnd_dnam	B_ex_dnam
#define extnd_dlb	B_ex_dlb
#define extnd_ilb	B_ex_ilb
#define extnd_hol	B_ex_hol
#define extnd_part	B_ex_part
#define extnd_cont	B_ex_cont
#define extnd_main	B_ex_main
#define gen1		B_gen1
#define gen2		B_gen2
#define gen4		B_gen4
#define save_label	B_save_label
#define dump_label	B_dump_label
#define align_word	B_align_word
#define reloc1		B_rlc1
#define reloc2		B_rlc2
#define reloc4		B_rlc4
#define set_global_visible	B_stglobvis
#define set_local_visible	B_stlocvis
#define symbol_definition	B_symdef
#define switchseg		B_switchseg
#define not_implemented		B_not_implemented

extern File *codefile;

extern char *extnd_name(), *extnd_dnam(), *extnd_dlb(), *extnd_ilb(),
	    *extnd_hol(), *extnd_ext(), *extnd_pro(), *extnd_start(),
	    *extnd_part(), *extnd_cont(), *extnd_main();

#define 	swtxt() 	switchseg( SEGTXT)
#define		bss(n)		fprint(codefile, BSS_FMT, (long)(n))

#define 	SEGTXT		0
#define 	SEGROM		1
#define 	SEGCON		2
#define 	SEGBSS		3
#define 	SEGHOL		-1	/* Does not exist */

#define 	ABSOLUTE	1
