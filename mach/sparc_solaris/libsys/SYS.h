/*
SYS.h
*/

#ifndef SYS_H
#define SYS_H

#define SYS_call_0(x) \
	LABEL_(x); \
	BODY(x)

#define _SYS_call_0(x) \
	LABEL__(x); \
	BODY(x)

#define SYS_call_1(x) \
	LABEL_(x); \
	ENTRY1; \
	BODY(x)

#define _SYS_call_1(x) \
	LABEL__(x); \
	ENTRY1; \
	BODY(x)

#define SYS_call_2(x) \
	LABEL_(x); \
	ENTRY2; \
	BODY(x)

#define _SYS_call_2(x) \
	LABEL__(x); \
	ENTRY2; \
	BODY(x)

#define SYS_call_3(x) \
	LABEL_(x); \
	ENTRY3; \
	BODY(x)

#define _SYS_call_3(x) \
	LABEL__(x); \
	ENTRY3; \
	BODY(x)

#define SYS_call_4(x) \
	LABEL_(x); \
	ENTRY4; \
	BODY(x)

#define _SYS_call_4(x) \
	LABEL__(x); \
	ENTRY4; \
	BODY(x)

#define SYS_call_5(x) \
	LABEL_(x); \
	ENTRY5; \
	BODY(x)

#define _SYS_call_5(x) \
	LABEL__(x); \
	ENTRY5; \
	BODY(x)

#define SYS_call_6(x) \
	LABEL_(x); \
	ENTRY6; \
	BODY(x)

#define _SYS_call_6(x) \
	LABEL__(x); \
	ENTRY6; \
	BODY(x)

#define BODY(x) \
	b x; \
	nop

#if __STDC__
#define LABEL_(x) \
	.global $ ## x; \
	$ ## x:
#define LABEL__(x) \
	.global $_ ## x; \
	$_ ## x:
#else
#define LABEL_(x) \
	.global $/**/x; \
	$/**/x:
#define LABEL__(x) \
	.global $_/**/x; \
	$_/**/x:
#endif

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

#endif /* SYS_H */
