	.globl _argv
	.data
	.align 4
	.comm	argstr,256,4
_argv:	.long	0

	.text
	.globl start
start:
	# clear bss (could be done better)
	# is it actually necessary?
	mov	$__bss,%eax
1:
	movb	$0,(%eax)
	inc	%eax
	cmp	$__ebss,%eax
	jbe	1b

	# copy command line args (can't use them directly, not aligned)
	mov	%esp,%eax
	shr	$2,%eax
	mov	%eax,_argv
	mov	(%esp),%ecx	# number of arguments
	mov	$argstr,%edi
1:
	mov	(%esp,%ecx,4),%esi
	mov	%edi,%eax
	shr	$2,%eax
	mov	%eax,(%esp,%ecx,4)
	call	cpystr
	loop	1b

	call	bsymbs
	mov	_main,%eax
	shl	$2,%eax
	call	*%eax
	mov	%eax,%ebx
	mov	$1,%eax
	int	$0x80

# copy string from esi to edi and convert '\0' to B's '*e', align edi
cpystr:
	mov	(%esi),%al
	test	%al,%al
	jz	1f
	mov	%al,(%edi)
	inc	%edi
	inc	%esi
	jmp	cpystr
1:
	movb	$04,(%edi)
	inc	%edi
	add	$3,%edi
	and	$~3,%edi
	ret

# shift addresses filled in by the linker 2 bits to the right
# so B only ever sees word addresses
bsymbs:
	mov	$__bsymb,%eax
1:
	cmp	$__ebsymb,%eax
	jge	1f
	mov	(%eax),%ebx
	mov	(%ebx),%ecx
	shr	$2,%ecx
	mov	%ecx,(%ebx)
	add	$4,%eax
	jmp	1b
1:
	ret

	.globl retrn
retrn:
	mov	%ebp,%esp
	pop	%ebp
	ret

# handle switch table:
# eax has the value, ebx the address of the switch table
	.globl bswitch
bswitch:
	xor	%ecx,%ecx
1:
	mov	(%ebx,%ecx,8),%edx
	mov	4(%ebx,%ecx,8),%edi
	test	%edi,%edi
	jz	1f		# default (last in table)
	cmp	%eax,%edx
	je	2f
	inc	%ecx
	jmp	1b
1:
	jmp	*%edx
2:
	jmp	*%edi

#
# Library functions in assembly
#
	.globl _exit
	.data
	.align 4
	.section .bsymb; .long _exit; .data
_exit:	.long	1f
	.text
	.align 4
1:	mov	$1,%eax
	mov	$0,%ebx
	int	$0x80

	.globl _write
	.data
	.align 4
	.section .bsymb; .long _write; .data
_write:	.long	1f
	.text
	.align 4
1:	mov	4(%esp),%ebx
	mov	8(%esp),%ecx
	shl	$2,%ecx
	mov	12(%esp),%edx
	mov	$4,%eax
	int	$0x80
	ret

	.globl _read
	.data
	.align 4
	.section .bsymb; .long _read; .data
_read:	.long	1f
	.text
	.align 4
1:	mov	4(%esp),%ebx
	mov	8(%esp),%ecx
	shl	$2,%ecx
	mov	12(%esp),%edx
	mov	$3,%eax
	int	$0x80
	ret

	.globl	_inv
	.data
	.align 4
	.section .bsymb; .long _inv; .data
_inv:	.long	1f
	.text
	.align 4
1:	mov	4(%esp),%eax
	not	%eax
	ret
