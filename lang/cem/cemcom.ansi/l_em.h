/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

/*
 * This file can be considered the em_code.h file of lint.
 * Those code generating functions that are used by cem and that have not
 * been defined away by #ifdef LINT, are defined away here. Note that this a
 * fairly random collection. E.g. it does not include C_open(), since the
 * standard C-open() C_close() sequence is protected by #ifdef LINT, but it
 * does include C_close() since the latter is also called in other places,
 * to terminate the compilation process.
 */

#define	store_block(sz, al)
#define	load_block(sz, al)

#define	C_asp(c)
#define	C_bra(b)
#define	C_cal(p)
#define	C_csa(w)
#define	C_csb(w)
#define	C_fil_dlb(g,o)
#define	C_lae_dlb(g,o)
#define	C_lal(c)
#define	C_lin(c)
#define	C_loi(c)
#define	C_lol(c)
#define	C_ret(c)
#define	C_sdl(c)
#define	C_sti(c)
#define	C_stl(c)

#define	C_busy()	0
#define	C_close()

#define	C_df_dlb(l)
#define	C_df_dnam(s)
#define	C_df_ilb(l)

#define	C_pro_narg(s)
#define	C_end(l)

#define	C_exa_dnam(s)
#define	C_ina_dnam(s)
#define	C_ina_dlb(l)
#define	C_exp(s)
#define	C_inp(s)

#define	C_bss_cst(n,w,i)

#define	C_con_cst(v)
#define	C_con_icon(v,s)
#define	C_con_ucon(v,s)
#define	C_con_fcon(v,s)
#define	C_con_scon(v,s)
#define	C_con_dnam(v,s)
#define	C_con_dlb(v,s)
#define	C_con_pnam(v)

#define	C_rom_cst(v)
#define	C_rom_icon(v,s)
#define	C_rom_scon(v,s)
#define	C_rom_ilb(v)

#define	C_ldl(l)

#define	C_mes_begin(ms)
#define	C_mes_end()

#define	C_ms_gto()
#define	C_ms_par(b)
#define	C_ms_reg(o,s,t,c)
#define	C_ms_err()

