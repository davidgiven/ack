.define fpac, fpadd, fpcdf, fpcfd, fpcfi, fpcif, fpcmf, fpcomp
.define fpdiv, fpfef, fpfif, fpmult, fpop, fpsub, fpnorm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
! floating point pakket voor Z80
! geimplementeerd zoals beschreven in 
! Electronica top internationaal.
! September 1979
! Auteur: Hr. R. Beverdam, Zuidbroekweg 9,7642 NW  Wierden

xa:	.space 1
fpac:
fal:	.space 1
fan:	.space 1
fam:	.space 1
fax:	.space 1
xo:	.space 1
fpop:
fol:	.space 1
fon:	.space 1
fom:	.space 1
fox:	.space 1

fpsub:
	call fpcomp		! inverteer fpacc
fpadd:
	ld de,(fam)		! d fax,e fam
	ld bc,(fom)		! b fox,c fom
	ld a,e			! test fpacc
	or a			! 0?
	jr z,movop		! ja: som=fpop dus verplaats
	xor a
	add a,c
	ret z			! som is dus fpacc, klaar
	ld a,b
	sub d			! a:=fox-fax
	ld l,a			! bewaar verschil exponenten
	jp p,skpneg		! maak positief
	neg
skpneg:
	cp 0x18			! verschil meer dan 23?
	ld a,l
	jp m,lineup		! spring indien binnen bereik
	and a			! getallen te groot tov elkaar
	ret m			! klaar als fpacc het grootst
movop:
	ld hl,fol		! verplaats fpop naar fpacc
	ld de,fal		! want fpop is het antwoord
	ld bc,4
	ldir
	ret
lineup:
	and a			! kijk welke groter is
	jp m,shifto		! spring als fpop>fpac
	inc a			! bereken sa
	ld b,a			! save sa in b register
	ld a,1			! so 1
	push af			! bewaar so op stapel
	jr shacop		! gr schuiven
shifto:
	neg			! bereken fox-fax
eqexp:
	inc a			! so 1+(fox-fax)
	push af			! bewaar so op stapel
	ld b,1			! sa 1
shacop:
	ld hl,(fal)		! l fal,h fan
	xor a			! xa 0
moracc:
	sra e			! schuif fam
	rr h			! fan
	rr l			! fal
	rra			! xa
	inc d			! update voor fax
	djnz moracc		! herhaal sa keer
	ld (xa),a		! berg alles
	ld (fal),hl		! weg in
	ld (fam),de		! fpacc en xa
	pop af			! haal so terug van stapel
	ld b,a			! en zet in b register
	xor a			! xo 0
	ld hl,(fol)		! l fol,h fon
morop:
	sra c			! schuif: fom
	rr h			!         fon
	rr l			!         
	rra			!         xo
	djnz morop		! herhaal so keer
	ld (xo),a
	ld (fol),hl
	ld (fom),bc		! berg alles weg in fpop en xo
	ld de,xa
	ld hl,xo
	ld b,4
	or a			! reset carry
addmor:
	ld a,(de)		! haal een byte
	adc a,(hl)		! tel er een bij op
	ld (de),a		! en berg de som weer op
	inc e
	inc l
	djnz addmor		! herhaal dit 4 keer
	jr fpnorm

yyy:
	.data2 fom

fpmult:
	call setsgn
	add a,(hl)		! bereken exponent produkt
	ld (hl),a		! fax exponent produkt
	ld a,(yyy)
	ld l,a
	ex de,hl		! gebruik de als wijzer
	xor a
	ld h,a
	ld l,a			! hoogste 16 bits van pp worden nul
	exx
	ld bc,(fal)
	ld de,(fam)		! haal mc in registers
	ld d,a			! d:=0 tbv 16-bit add
	ld h,a
	ld l,a			! middelste 16 bits van pp worden nul
	ld ix,0			! laagste 16 bits ook
	exx
	ld c,3
mult:
	ld a,(de)		! haal een byte van mr
	dec e
	ld b,8			! bits in a byte
shift:
	rla			! schuif vooste bit in carry
	exx
	jr nc,noadd		! vooste bit is 0, dan niet optellen
	add ix,bc		! pp:=pp+mc
	adc hl,de		! continued
noadd:
	add ix,ix
	adc hl,hl
	exx
	adc hl,hl		! dit schoof het hele partiele produkt <
	djnz shift		! herhaal voor alle 8 bits
	dec c
	jr nz,mult		! herhaal voor 3 bytes
	exx
	rl l
	rla
	add a,h
	ld (fal),a
	ld a,d
	exx
	adc a,l
	ld (fan),a		! rond getal in pp af en berg resultaat op
	ld a,c
	adc a,h
	ld (fam),a
	call fpnorm
exmldv:
	ld hl,xa
	ld c,(hl)
	jp resign		! fix sign

fpdiv:
	call setsgn
	sub (hl)
	ld (hl),a		! berg exponent quotient op
	ld hl,(fol)
	push hl
	pop ix
	ld de,(fal)
	ld a,(fam)
	or a			! fpacc = 0 ?
	jr z,fperr		! fout, deling door nul
	ld b,a			! b:=fam
	ld a,(fom)
	ld c,a
	exx
	ld hl,fam
	ld e,3
divide:
	ld b,8
mordiv:
	exx
	and a
	sbc hl,de
	sbc a,b			! probeer de aftrekking
	jp m,nogo		! gaat niet
	push hl
	pop ix
	ld c,a
	ex af,af2		! quotient in tweede accumulator
	scf
	jr quorot
nogo:
	ex af,af2
	or a
quorot:
	rla			! volgende bit in quotient
	ex af,af2
	add ix,ix		! schuif eventueel vernieuwde
	rl c			! dd naar links
	push ix
	pop hl
	ld a,c			! zet nieuwe dd in rekenregisters
	exx
	djnz mordiv		! herhaal 8 keer
	ex af,af2
	ld (hl),a		! zet een byte van het quotient in het geheugen
	dec l
	ex af,af2
	dec e
	jr nz,divide		! herhaal 3 keer
	ld bc,(fal)
	ld hl,(fam)		! haal quotient terug in cpu
	bit 7,l
	jp z,exmldv		! als niet te groot tekenherstellen
	ld a,1			! wel te groot
	add a,c			! eerst getal afronden
	ld c,a
	ld a,e
	adc a,b
	ld b,a
	ld a,e
	adc a,l
	ld l,a
shft:
	inc h			! nu getal naar rechts schuiven
	rr l
	rr b
	rr c
	or a
	bit 7,l
	jr nz,shft		! door afronding weer te groot
	ld (fal),bc
	ld (fam),hl
	jr exmldv		! inspecteer teken
setsgn:
	ld a,(fom)		! ******** setsgn ************
	ld c,1			! teken -1
	rlca			! fpop 0 ?
	jr nc,tstacc		! nee
	rrc c			! ja, dus teken:=teken*(-1)
	ld hl,fol		! en inverteer fpop
	call complm
tstacc:
	ld a,(fam)
	rlca			! fpacc 0?
	jr nc,init		! nee
	rrc c			! ja dus teken:=teken*(-1)
	call fpcomp
init:
	ld hl,xa		! initialiseer nog een paar registers
	ld (hl),c
	ld a,(xxx)
	ld l,a
	ld a,(fox)
	ret
xxx:
	.data2 fax

fpcif:
	ld de,(fpac)		! integer to convert
	xor a
	sra d
	rr e
	rr a
	ld (fan),de
	ld (fal),a
	ld a,16
	ld (fax),a
	jr fpnorm

fpcfi:
	ld a,(fax)
	dec a
	jp m,fpzero		! really integer zero here
	sub 15
	jp p,fperr		! overflow
	ld de,(fan)
	inc a
	neg
	jr z,2f
	ld b,a
	ld a,(fal)
1:
	sra d
	rr e
	rr a
	djnz 1b
2:
	bit 7,d
	jr z,0f
	inc de
0:
	ld (fpac),de
	ret

fpcdf:
	ld de,(fpac)
	ld bc,(fpac+2)
	ld h,31
3:
	ld a,b
	and 0300
	jr z,1f
	cp 0300
	jr z,1f
	or a
	jp p,2f
	sra b
	rr c
	rr d
	inc h
2:
	ld a,h
	ld (fax),a
	ld (fan),bc
	ld a,d
	ld (fal),a
	ret
1:
	sla e
	rl d
	rl c
	rl b
	dec h
	jr 3b

fpcfd:
	ld a,(fax)
	dec a
	jp m,fpzero
	cp 32
	jp p,fperr
	sub 31
	cpl
	ld bc,(fan)
	ld de,(fal)
	ld d,e
	ld e,0
1:
	dec a
	jp m,2f
	sra b
	rr c
	rr d
	rr e
	jr 1b
2:
	bit 7,b
	jr z,3f
	sla e
	rl d
	rl c
	rl b
3:
	ld (fpac+2),bc
	ld (fpac),de
	ret
fpfef:
	ld a,(fox)
	ld (fpac),a
9:
	bit 7,a
	jr z,1f
	ld a,0xFF
	jr 2f
1:
	xor a
2:
	ld (fpac+1),a
	xor a
	ld (fox),a
	ret
fpcmf:
	call fpsub
	ld a,(fam)
	ld (fpac),a
	jr 9b
fpfif:
	call fpmult
	ld a,(fax)
	dec a
	jp m,intzero
	inc a
	ld b,a
	xor a
	ld c,0200
	ld d,a
	ld e,a
1:
	sra c
	rr d
	rr e
	djnz 1b
	ld hl,fam
	ld b,(hl)
	ld a,c
	and b
	ld (fom),a
	ld a,c
	xor 0177
	and b
	ld (hl),a
	dec l
	ld b,(hl)
	ld a,d
	and b
	ld (fon),a
	ld a,d
	cpl
	and b
	ld (hl),a
	dec l
	ld b,(hl)
	ld a,e
	and b
	ld (fol),a
	ld a,e
	cpl
	and b
	ld (hl),a
	ld a,(fax)
	ld (fox),a
	jr fpnorm
intzero:
	xor a
	ld hl,fol
	ld b,4
1:	ld (hl),a
	inc hl
	djnz 1b
	ret

fpzero:
	xor a
	ld h,a
	ld l,a
	ld (fal),hl
	ld (fam),hl
	ret

fpnorm:
	ld a,(fam)
	ld c,a
	or a			! fpacc < 0 ?
	call m,fpcomp		! ja -- inverteer
	ld hl,(fal)
	ld de,(fam)
	ld a,l
	or h
	or e
	jr z,fpzero		! als hele facc 0 is
	ld a,e
mortst:
	bit 6,a			! test meest significante bit
	jr nz,catch		! stop als bit is 1
	add hl,hl		! schuif links zolang bit = 0
	adc a,a
	dec d			! pas fax ook aan
	jr mortst
catch:
	ld e,a			! herstel nu fpacc in geheugen
	ld (fal),hl
	ld (fam),de
resign:
	bit 7,c			! test op teken
	ret z			! positief, geen actie
fpcomp:
	ld hl,fal
complm:
	ld b,3			! inverteer alleen mantisse
	xor a
morcom:
	sbc a,(hl)
	ld (hl),a
	inc hl
	ld a,0
	djnz morcom
	or a
	ret
fperr:
	scf
	ret
