.define	.gto
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

!r0 contains descriptor addres
.gto:	
	addr 0(sp), r7		!first put future localbase in sp
	subd 8(r0), r7
	subd 4, r7
	adjspd r7		!done
	movd -4(sp), r1		!save this memory location
	enter[], 0		!adjust local base
	movd r1, 0(sp)		!restore saved memory location
	addr 0(sp), r7		!adjust stackpointer now
	subd 4(r0), r7
	adjspd r7
	jump r0			!adjust programcounter
