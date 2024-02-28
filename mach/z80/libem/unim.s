.define unimpld, e.mon, e.rck, .trp.z, .unimpld
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.unimpld:
unimpld:		! used in dispatch table to
			! catch unimplemented instructions
	ld hl,EILLINS
9:	push hl
	call .trp.z
	ret

e.mon:
	ld hl,EBADMON
	jr 9b
e.rck:
	push af
	ld a,(.ignmask)
	bit 1,a
	jr nz,8f
	ld hl,ERANGE
	jr 9b
8:
	pop af
	ret


