df_dlb	| label:l	|
	FLUSHDFA();
	O_df_dlb(l);
df_dnam	| char *:s	|
	FLUSHDFA();
	O_df_dnam(s);
pro	| char *:s arith:l	|
	FLUSHDFA();
	O_pro(s,l);
pro_narg	| char *:s	|
	FLUSHDFA();
	O_pro_narg(s);
end	| arith:l	|
	FLUSHDFA();
	O_end(l);
end_narg	|	|
	FLUSHDFA();
	O_end_narg();
exa_dnam	| char *:s	|
	FLUSHDFA();
	O_exa_dnam(s);
exa_dlb	| label:l	|
	FLUSHDFA();
	O_exa_dlb(l);
exp	| char *:s	|
	FLUSHDFA();
	O_exp(s);
ina_dnam	| char *:s	|
	FLUSHDFA();
	O_ina_dnam(s);
ina_dlb	| label:l	|
	FLUSHDFA();
	O_ina_dlb(l);
inp	| char *:s	|
	FLUSHDFA();
	O_inp(s);
bss_cst	| arith:n arith:w int:i	|
	FLUSHDFA();
	O_bss_cst(n,w,i);
bss_icon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	O_bss_icon(n,s,sz,i);
bss_ucon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	O_bss_ucon(n,s,sz,i);
bss_fcon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	O_bss_fcon(n,s,sz,i);
bss_dnam	| arith:n char *:s arith:offs int:i	|
	FLUSHDFA();
	O_bss_dnam(n,s,offs,i);
bss_dlb	| arith:n label:l arith:offs int:i	|
	FLUSHDFA();
	O_bss_dlb(n,l,offs,i);
bss_ilb	| arith:n label:l int:i	|
	FLUSHDFA();
	O_bss_ilb(n,l,i);
bss_pnam	| arith:n char *:s int:i	|
	FLUSHDFA();
	O_bss_pnam(n,s,i);
hol_cst	| arith:n arith:w int:i	|
	FLUSHDFA();
	O_hol_cst(n,w,i);
hol_icon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	O_hol_icon(n,s,sz,i);
hol_ucon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	O_hol_ucon(n,s,sz,i);
hol_fcon	| arith:n char *:s arith:sz int:i	|
	FLUSHDFA();
	O_hol_fcon(n,s,sz,i);
hol_dnam	| arith:n char *:s arith:offs int:i	|
	FLUSHDFA();
	O_hol_dnam(n,s,offs,i);
hol_dlb	| arith:n label:l arith:offs int:i	|
	FLUSHDFA();
	O_hol_dlb(n,l,offs,i);
hol_ilb	| arith:n label:l int:i	|
	FLUSHDFA();
	O_hol_ilb(n,l,i);
hol_pnam	| arith:n char *:s int:i	|
	FLUSHDFA();
	O_hol_pnam(n,s,i);
con_cst	| arith:l	|
	FLUSHDFA();
	O_con_cst(l);
con_icon	| char *:val arith:siz	|
	FLUSHDFA();
	O_con_icon(val,siz);
con_ucon	| char *:val arith:siz	|
	FLUSHDFA();
	O_con_ucon(val,siz);
con_fcon	| char *:val arith:siz	|
	FLUSHDFA();
	O_con_fcon(val,siz);
con_scon	| char *:str arith:siz	|
	FLUSHDFA();
	O_con_scon(str,siz);
con_dnam	| char *:str arith:val	|
	FLUSHDFA();
	O_con_dnam(str,val);
con_dlb	| label:l arith:val	|
	FLUSHDFA();
	O_con_dlb(l,val);
con_ilb	| label:l	|
	FLUSHDFA();
	O_con_ilb(l);
con_pnam	| char *:str	|
	FLUSHDFA();
	O_con_pnam(str);
rom_cst	| arith:l	|
	FLUSHDFA();
	O_rom_cst(l);
rom_icon	| char *:val arith:siz	|
	FLUSHDFA();
	O_rom_icon(val,siz);
rom_ucon	| char *:val arith:siz	|
	FLUSHDFA();
	O_rom_ucon(val,siz);
rom_fcon	| char *:val arith:siz	|
	FLUSHDFA();
	O_rom_fcon(val,siz);
rom_scon	| char *:str arith:siz	|
	FLUSHDFA();
	O_rom_scon(str,siz);
rom_dnam	| char *:str arith:val	|
	FLUSHDFA();
	O_rom_dnam(str,val);
rom_dlb	| label:l arith:val	|
	FLUSHDFA();
	O_rom_dlb(l,val);
rom_ilb	| label:l	|
	FLUSHDFA();
	O_rom_ilb(l);
rom_pnam	| char *:str	|
	FLUSHDFA();
	O_rom_pnam(str);
cst	| arith:l	|
	FLUSHDFA();
	O_cst(l);
icon	| char *:val arith:siz	|
	FLUSHDFA();
	O_icon(val,siz);
ucon	| char *:val arith:siz	|
	FLUSHDFA();
	O_ucon(val,siz);
fcon	| char *:val arith:siz	|
	FLUSHDFA();
	O_fcon(val,siz);
scon	| char *:str arith:siz	|
	FLUSHDFA();
	O_scon(str,siz);
dnam	| char *:str arith:val	|
	FLUSHDFA();
	O_dnam(str,val);
dlb	| label:l arith:val	|
	FLUSHDFA();
	O_dlb(l,val);
ilb	| label:l	|
	FLUSHDFA();
	O_ilb(l);
pnam	| char *:str	|
	FLUSHDFA();
	O_pnam(str);
mes_begin	| int:ms	|
	FLUSHDFA();
	O_mes_begin(ms);
mes_end	|	|
	FLUSHDFA();
	O_mes_end();
exc	| arith:c1 arith:c2	|
	FLUSHDFA();
	O_exc(c1,c2);
