EM = ../../..
CEGLIB = $(EM)/lib/ceg
DEF = $(CEGLIB)/defaults
MessageList = C_cst.c C_dlb.c C_dnam.c C_fcon.c C_icon.c C_ilb.c C_mes_begin.c \
		C_mes_end.c C_pnam.c C_scon.c C_ucon.c
NotimplList = not_impl.c not_impl_table
PseudoList = C_busy.c C_close.c C_df_dlb.c C_df_dnam.c C_df_ilb.c C_end.c \
		C_end_narg.c C_exa_dlb.c C_exa_dnam.c C_exp.c C_ina_dlb.c \
		C_ina_dnam.c C_init.c C_inp.c C_magic.c C_open.c C_pro.c \
		C_pro_narg.c C_insertpart.c
StorageList = C_bss_cst.c C_bss_dlb.c C_bss_dnam.c C_bss_ilb.c C_bss_pnam.c \
		C_con_cst.c C_con_dlb.c C_con_dnam.c C_con_ilb.c C_con_pnam.c \
		C_con_scon.c C_hol_cst.c C_hol_dlb.c C_hol_dnam.c C_hol_ilb.c \
		C_hol_pnam.c C_rom_cst.c C_rom_dlb.c C_rom_dnam.c C_rom_ilb.c \
		C_rom_pnam.c C_rom_scon.c

all:	C_out.c

clean:
	rm -f C_out.c C_mnem C_mnem_narg

install:	all
	-mkdir $(CEGLIB)
	-mkdir $(DEF)
	-mkdir $(DEF)/message
	-mkdir $(DEF)/not_impl
	-mkdir $(DEF)/pseudo
	-mkdir $(DEF)/storage
	cp pseudo_vars.c $(DEF)
	for i in $(MessageList) ; do cp message/$$i $(DEF)/message/$$i ; done
	for i in $(NotimplList) ; do cp not_impl/$$i $(DEF)/not_impl/$$i ; done
	for i in $(PseudoList) ; do cp pseudo/$$i $(DEF)/pseudo/$$i ; done
	for i in $(StorageList) ; do cp storage/$$i $(DEF)/storage/$$i ; done
	cp C_out.c $(DEF)/C_out.c

cmp:
	-cmp pseudo_vars.c $(DEF)/pseudo_vars.c
	-for i in $(MessageList) ; do cmp message/$$i $(DEF)/message/$$i ; done
	-for i in $(NotimplList) ; do cmp not_impl/$$i $(DEF)/not_impl/$$i ; done
	-for i in $(PseudoList) ; do cmp pseudo/$$i $(DEF)/pseudo/$$i ; done
	-for i in $(StorageList) ; do cmp storage/$$i $(DEF)/storage/$$i ; done
	-cmp C_out.c $(DEF)/C_out.c

pr:
	@for i in $(MessageList) ; do pr message/$$i ; done
	@for i in $(NotimplList) ; do pr not_impl/$$i ; done
	@for i in $(PseudoList) ; do pr pseudo/$$i ; done
	@for i in $(StorageList) ; do pr storage/$$i ; done

opr:
	make pr | opr

C_out.c:	C_out_skel.c C_mnem C_mnem_narg mk_C_out
	mk_C_out > C_out.c

C_mnem:		m_C_mnem argtype
		sh m_C_mnem > C_mnem

C_mnem_narg:	m_C_mnem_na argtype
		sh m_C_mnem_na > C_mnem_narg
