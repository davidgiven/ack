#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "nopt.h"

C_df_dlb(l)
	label l;
{
	FLUSHDFA();
	O_df_dlb(l);
}

C_df_dnam(s)
	char * s;
{
	FLUSHDFA();
	O_df_dnam(s);
}

C_pro(s,l)
	char * s;
	arith l;
{
	FLUSHDFA();
	O_pro(s,l);
}

C_pro_narg(s)
	char * s;
{
	FLUSHDFA();
	O_pro_narg(s);
}

C_end(l)
	arith l;
{
	FLUSHDFA();
	O_end(l);
}

C_end_narg()
{
	FLUSHDFA();
	O_end_narg();
}

C_exa_dnam(s)
	char * s;
{
	FLUSHDFA();
	O_exa_dnam(s);
}

C_exa_dlb(l)
	label l;
{
	FLUSHDFA();
	O_exa_dlb(l);
}

C_exp(s)
	char * s;
{
	FLUSHDFA();
	O_exp(s);
}

C_ina_dnam(s)
	char * s;
{
	FLUSHDFA();
	O_ina_dnam(s);
}

C_ina_dlb(l)
	label l;
{
	FLUSHDFA();
	O_ina_dlb(l);
}

C_inp(s)
	char * s;
{
	FLUSHDFA();
	O_inp(s);
}

C_bss_cst(n,w,i)
	arith n;
	arith w;
	int i;
{
	FLUSHDFA();
	O_bss_cst(n,w,i);
}

C_bss_icon(n,s,sz,i)
	arith n;
	char * s;
	arith sz;
	int i;
{
	FLUSHDFA();
	O_bss_icon(n,s,sz,i);
}

C_bss_ucon(n,s,sz,i)
	arith n;
	char * s;
	arith sz;
	int i;
{
	FLUSHDFA();
	O_bss_ucon(n,s,sz,i);
}

C_bss_fcon(n,s,sz,i)
	arith n;
	char * s;
	arith sz;
	int i;
{
	FLUSHDFA();
	O_bss_fcon(n,s,sz,i);
}

C_bss_dnam(n,s,offs,i)
	arith n;
	char * s;
	arith offs;
	int i;
{
	FLUSHDFA();
	O_bss_dnam(n,s,offs,i);
}

C_bss_dlb(n,l,offs,i)
	arith n;
	label l;
	arith offs;
	int i;
{
	FLUSHDFA();
	O_bss_dlb(n,l,offs,i);
}

C_bss_ilb(n,l,i)
	arith n;
	label l;
	int i;
{
	FLUSHDFA();
	O_bss_ilb(n,l,i);
}

C_bss_pnam(n,s,i)
	arith n;
	char * s;
	int i;
{
	FLUSHDFA();
	O_bss_pnam(n,s,i);
}

C_hol_cst(n,w,i)
	arith n;
	arith w;
	int i;
{
	FLUSHDFA();
	O_hol_cst(n,w,i);
}

C_hol_icon(n,s,sz,i)
	arith n;
	char * s;
	arith sz;
	int i;
{
	FLUSHDFA();
	O_hol_icon(n,s,sz,i);
}

C_hol_ucon(n,s,sz,i)
	arith n;
	char * s;
	arith sz;
	int i;
{
	FLUSHDFA();
	O_hol_ucon(n,s,sz,i);
}

C_hol_fcon(n,s,sz,i)
	arith n;
	char * s;
	arith sz;
	int i;
{
	FLUSHDFA();
	O_hol_fcon(n,s,sz,i);
}

C_hol_dnam(n,s,offs,i)
	arith n;
	char * s;
	arith offs;
	int i;
{
	FLUSHDFA();
	O_hol_dnam(n,s,offs,i);
}

C_hol_dlb(n,l,offs,i)
	arith n;
	label l;
	arith offs;
	int i;
{
	FLUSHDFA();
	O_hol_dlb(n,l,offs,i);
}

C_hol_ilb(n,l,i)
	arith n;
	label l;
	int i;
{
	FLUSHDFA();
	O_hol_ilb(n,l,i);
}

C_hol_pnam(n,s,i)
	arith n;
	char * s;
	int i;
{
	FLUSHDFA();
	O_hol_pnam(n,s,i);
}

C_con_cst(l)
	arith l;
{
	FLUSHDFA();
	O_con_cst(l);
}

C_con_icon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_con_icon(val,siz);
}

C_con_ucon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_con_ucon(val,siz);
}

C_con_fcon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_con_fcon(val,siz);
}

C_con_scon(str,siz)
	char * str;
	arith siz;
{
	FLUSHDFA();
	O_con_scon(str,siz);
}

C_con_dnam(str,val)
	char * str;
	arith val;
{
	FLUSHDFA();
	O_con_dnam(str,val);
}

C_con_dlb(l,val)
	label l;
	arith val;
{
	FLUSHDFA();
	O_con_dlb(l,val);
}

C_con_ilb(l)
	label l;
{
	FLUSHDFA();
	O_con_ilb(l);
}

C_con_pnam(str)
	char * str;
{
	FLUSHDFA();
	O_con_pnam(str);
}

C_rom_cst(l)
	arith l;
{
	FLUSHDFA();
	O_rom_cst(l);
}

C_rom_icon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_rom_icon(val,siz);
}

C_rom_ucon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_rom_ucon(val,siz);
}

C_rom_fcon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_rom_fcon(val,siz);
}

C_rom_scon(str,siz)
	char * str;
	arith siz;
{
	FLUSHDFA();
	O_rom_scon(str,siz);
}

C_rom_dnam(str,val)
	char * str;
	arith val;
{
	FLUSHDFA();
	O_rom_dnam(str,val);
}

C_rom_dlb(l,val)
	label l;
	arith val;
{
	FLUSHDFA();
	O_rom_dlb(l,val);
}

C_rom_ilb(l)
	label l;
{
	FLUSHDFA();
	O_rom_ilb(l);
}

C_rom_pnam(str)
	char * str;
{
	FLUSHDFA();
	O_rom_pnam(str);
}

C_cst(l)
	arith l;
{
	FLUSHDFA();
	O_cst(l);
}

C_icon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_icon(val,siz);
}

C_ucon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_ucon(val,siz);
}

C_fcon(val,siz)
	char * val;
	arith siz;
{
	FLUSHDFA();
	O_fcon(val,siz);
}

C_scon(str,siz)
	char * str;
	arith siz;
{
	FLUSHDFA();
	O_scon(str,siz);
}

C_dnam(str,val)
	char * str;
	arith val;
{
	FLUSHDFA();
	O_dnam(str,val);
}

C_dlb(l,val)
	label l;
	arith val;
{
	FLUSHDFA();
	O_dlb(l,val);
}

C_ilb(l)
	label l;
{
	FLUSHDFA();
	O_ilb(l);
}

C_pnam(str)
	char * str;
{
	FLUSHDFA();
	O_pnam(str);
}

C_mes_begin(ms)
	int ms;
{
	FLUSHDFA();
	O_mes_begin(ms);
}

C_mes_end()
{
	FLUSHDFA();
	O_mes_end();
}

C_exc(c1,c2)
	arith c1;
	arith c2;
{
	FLUSHDFA();
	O_exc(c1,c2);
}
