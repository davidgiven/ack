/*
SYS.h
*/

#ifndef SYS_H
#define SYS_H

#include <syscall.h>

#define SYS_call_0(x) \
	LABEL_(x); \
	BODY(x)

#define _SYS_call_0(x) \
	LABEL__(x); \
	LABEL(x); \
	BODY(x)

#define SYS_call_1(x) \
	LABEL_(x); \
	ENTRY1; \
	BODY(x)

#define _SYS_call_1(x) \
	LABEL__(x); \
	ENTRY1; \
	LABEL(x); \
	BODY(x)

#define SYS_call_2(x) \
	LABEL_(x); \
	ENTRY2; \
	BODY(x)

#define _SYS_call_2(x) \
	LABEL__(x); \
	ENTRY2; \
	LABEL(x); \
	BODY(x)

#define SYS_call_3(x) \
	LABEL_(x); \
	ENTRY3; \
	BODY(x)

#define _SYS_call_3(x) \
	LABEL__(x); \
	ENTRY3; \
	LABEL(x); \
	BODY(x)

#define SYS_call_4(x) \
	LABEL_(x); \
	ENTRY4; \
	BODY(x)

#define _SYS_call_4(x) \
	LABEL__(x); \
	ENTRY4; \
	LABEL(x); \
	BODY(x)

#define SYS_call_5(x) \
	LABEL_(x); \
	ENTRY5; \
	BODY(x)

#define _SYS_call_5(x) \
	LABEL__(x); \
	ENTRY5; \
	LABEL(x); \
	BODY(x)

#define SYS_call_6(x) \
	LABEL_(x); \
	ENTRY6; \
	BODY(x)

#define _SYS_call_6(x) \
	LABEL__(x); \
	ENTRY6; \
	LABEL(x); \
	BODY(x)

#define BODY1(x) \
	sys_call(x); \
	ta	%g0; \
	bgeu 0f; \
	sethi   %hi(cerror), %o5; \
	or      %o5, %lo(cerror), %o5; \
	jmp	%o5; \
	nop; \
0:;
#define BODY(x) \
	BODY1(x) \
	retl; \
	nop

#if __STDC__
#define LABEL_(x) \
	.global _ ## x; \
	_ ## x:
#define LABEL__(x) \
	.global __ ## x; \
	__ ## x:
#else
#define LABEL_(x) \
	.global _/**/x; \
	_/**/x:
#define LABEL__(x) \
	.global __/**/x; \
	__/**/x:
#endif
#define LABEL(x) \
	.global x; \
	x:

#define ENTRY1 \
	ld	[%l0], %o0 \

#define ENTRY2 \
	ENTRY1; \
	ld	[%l0+4], %o1 \

#define ENTRY3 \
	ENTRY2; \
	ld	[%l0+8], %o2 \

#define ENTRY4 \
	ENTRY3; \
	ld	[%l0+12], %o3 \

#define ENTRY5 \
	ENTRY4; \
	ld	[%l0+16], %o4 \

#define ENTRY6 \
	ENTRY5; \
	ld	[%l0+20], %o5 \

#if __STDC__
#define sys_call(x) \
	mov	SYS_ ## x, %g1
#else
#define sys_call(x) \
	mov	SYS_/**/x, %g1
#endif

#endif /* SYS_H */
