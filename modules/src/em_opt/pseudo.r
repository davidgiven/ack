df_dlb	| label:l	|
	register p_instr p = GETNXTPATT();
	FLUSHDFA(p);
	C_df_dlb(l);
df_dnam	| char *:s	|
	register p_instr p = GETNXTPATT();
	FLUSHDFA(p);
	C_df_dnam(s);
pro	| char *:s arith:l	|
	register p_instr p = GETNXTPATT();
	FLUSHDFA(p);
	C_pro(s,l);
pro_narg	| char *:s	|
	register p_instr p = GETNXTPATT();
	FLUSHDFA(p);
	C_pro_narg(s);
end	| arith:l	|
	register p_instr p = GETNXTPATT();
	FLUSHDFA(p);
	C_end(l);
end_narg	|	|
	FLUSHDFA();
	C_end_narg();
exa_dnam	| char *:s	|
	FLUSHDFA();
	C_exa_dnam(s);
exa_dlb	| label:l	|
	FLUSHDFA();
	C_exa_dlb(l);
exp	| char *:s	|
	FLUSHDFA();
	C_exp(s);
ina_dnam	| char *:s	|
	FLUSHDFA();
	C_ina_dnam(s);
ina_dlb	| label:l	|
	FLUSHDFA();
	C_ina_dlb(l);
inp	| char *:s	|
	FLUSHDFA();
	C_inp(s);
bss_cst	| arith:n arith:w int:i	|
	FLUSHDFA();
	C_bss_cst(n,w,i);
bss_icon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	C_bss_icon(n,s,sz,i);
bss_ucon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	C_bss_ucon(n,s,sz,i);
bss_fcon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	C_bss_fcon(n,s,sz,i);
bss_dnam	| arith:n char *:s arith:offs int:i	|
	FLUSHDFA();
	C_bss_dnam(n,s,offs,i);
bss_dlb	| arith:n label:l arith:offs int:i	|
	FLUSHDFA();
	C_bss_dlb(n,l,offs,i);
bss_ilb	| arith:n label:l int:i	|
	FLUSHDFA();
	C_bss_ilb(n,l,i);
bss_pnam	| arith:n char *:s int:i	|
	FLUSHDFA();
	C_bss_pnam(n,s,i);
hol_cst	| arith:n arith:w int:i	|
	FLUSHDFA();
	C_hol_cst(n,w,i);
hol_icon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	C_hol_icon(n,s,sz,i);
hol_ucon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	C_hol_ucon(n,s,sz,i);
hol_fcon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	C_hol_fcon(n,s,sz,i);
hol_dnam	| arith:n char *:s arith:offs int:i	|
	FLUSHDFA();
	C_hol_dnam(n,s,offs,i);
hol_dlb	| arith:n label:l arith:offs int:i	|
	FLUSHDFA();
	C_hol_dlb(n,l,offs,i);
hol_ilb	| arith:n label:l int:i	|
	FLUSHDFA();
	C_hol_ilb(n,l,i);
hol_pnam	| arith:n char *:s int:i	|
	FLUSHDFA();
	C_hol_pnam(n,s,i);
con_cst	| arith:l	|
	FLUSHDFA();
	C_con_cst(l);
con_icon	| char *:val arith:siz	|
	FLUSHDFA();
	C_con_icon(val,siz);
con_ucon	| char *:val arith:siz	|
	FLUSHDFA();
	C_con_ucon(val,siz);
con_fcon	| char *:val arith:siz	|
	FLUSHDFA();
	C_con_fcon(val,siz);
con_scon	| char *:str arith:siz	|
	FLUSHDFA();
	C_con_scon(str,siz);
con_dnam	| char *:str arith:val	|
	FLUSHDFA();
	C_con_dnam(str,val);
con_dlb	| label:l arith:val	|
	FLUSHDFA();
	C_con_dlb(l,val);
con_ilb	| label:l	|
	FLUSHDFA();
	C_con_ilb(l);
con_pnam	| char *:str	|
	FLUSHDFA();
	C_con_pnam(str);
rom_cst	| arith:l	|
	FLUSHDFA();
	C_rom_cst(l);
rom_icon	| char *:val arith:siz	|
	FLUSHDFA();
	C_rom_icon(val,siz);
rom_ucon	| char *:val arith:siz	|
	FLUSHDFA();
	C_rom_ucon(val,siz);
rom_fcon	| char *:val arith:siz	|
	FLUSHDFA();
	C_rom_fcon(val,siz);
rom_scon	| char *:str arith:siz	|
	FLUSHDFA();
	C_rom_scon(str,siz);
rom_dnam	| char *:str arith:val	|
	FLUSHDFA();
	C_rom_dnam(str,val);
rom_dlb	| label:l arith:val	|
	FLUSHDFA();
	C_rom_dlb(l,val);
rom_ilb	| label:l	|
	FLUSHDFA();
	C_rom_ilb(l);
rom_pnam	| char *:str	|
	FLUSHDFA();
	C_rom_pnam(str);
cst	| arith:l	|
	FLUSHDFA();
	C_cst(l);
icon	| char *:val arith:siz	|
	FLUSHDFA();
	C_icon(val,siz);
ucon	| char *:val arith:siz	|
	FLUSHDFA();
	C_ucon(val,siz);
fcon	| char *:val arith:siz	|
	FLUSHDFA();
	C_fcon(val,siz);
scon	| char *:str arith:siz	|
	FLUSHDFA();
	C_scon(str,siz);
dnam	| char *:str arith:val	|
	FLUSHDFA();
	C_dnam(str,val);
dlb	| label:l arith:val	|
	FLUSHDFA();
	C_dlb(l,val);
ilb	| label:l	|
	FLUSHDFA();
	C_ilb(l);
pnam	| char *:str	|
	FLUSHDFA();
	C_pnam(str);
mes_begin	| int:ms	|
	FLUSHDFA();
	C_mes_begin(ms);
mes_end	|	|
	FLUSHDFA();
	C_mes_end();
exc	| arith:c1 arith:c2	|
	FLUSHDFA();
	C_exc(c1,c2);
insertpart	| int:id	|
	FLUSHDFA();
	C_insertpart(id);
beginpart	| int:id	|
	FLUSHDFA();
	C_beginpart(id);
endpart	| int:id	|
	FLUSHDFA();
	C_endpart(id);
