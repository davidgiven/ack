#ifndef _TYPE_H
#define _TYPE_H
/* Macros */
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#define MIN(a,b)	((a) < (b) ? (a) : (b))

/* Type definitions */
typedef unsigned short	unshort;	/* must be 16-bit unsigned */
typedef unshort block_nr;	/* block number */
typedef unshort zone_nr;	/* zone number */

#define MAX_BLOCK_NR  ((block_nr) 0177777)	/* largest block number */
#define HIGHEST_ZONE   ((zone_nr) 0177777)	/* largest zone number */
#define MAX_INODE_NR      ((ino_t 0177777)	/* largest inode number */
#define MAX_FILE_POS	     017777777777L	/* largest legal file offset */

#define NO_BLOCK	    ((block_nr) 0)	/* absence of a block number */
#define NO_ENTRY	       ((ino_t) 0)	/* absence of a dir entry */
#define NO_ZONE		     ((zone_nr) 0)	/* absence of a zone number */
#define NO_DEV		      ((dev_t) ~0)	/* absence of a device numb */

typedef unshort bit_nr;		/* if ino_t & zone_nr both unshort,
				   then also unshort, else long */
typedef long zone_type;		/* zone size */

#if (CHIP == INTEL)
typedef unsigned vir_bytes;	/* virtual addresses and lengths in bytes */
#endif

#if (CHIP == M68000)
typedef long vir_bytes;		/* virtual addresses and lengths in bytes */
#endif

typedef unsigned vir_clicks;	/* virtual addresses and lengths in clicks */
typedef long phys_bytes;	/* physical addresses and lengths in bytes */
typedef unsigned phys_clicks;	/* physical addresses and lengths in clicks */
typedef int signed_clicks;	/* same length as phys_clicks, but signed */

/* Types relating to messages. */
#define M1		1
#define M3		3
#define M4		4
#define M3_STRING	14

typedef struct {int m1i1, m1i2, m1i3; char *m1p1, *m1p2, *m1p3;} mess_1;
typedef struct {int m2i1, m2i2, m2i3; long m2l1, m2l2; char *m2p1;} mess_2;
typedef struct {int m3i1, m3i2; char *m3p1; char m3ca1[M3_STRING];} mess_3;
typedef struct {long m4l1, m4l2, m4l3, m4l4;} mess_4;
typedef struct {char m5c1, m5c2; int m5i1, m5i2; long m5l1, m5l2, m5l3;} mess_5;
#if _ANSI
typedef struct {int m6i1, m6i2, m6i3; long m6l1; void (*m6f1)(int);} mess_6;
#else
typedef struct {int m6i1, m6i2, m6i3; long m6l1; void (*m6f1)();} mess_6;
#endif

typedef struct {
  int m_source;			/* who sent the message */
  int m_type;			/* what kind of message is it */
  union {
	mess_1 m_m1;
	mess_2 m_m2;
	mess_3 m_m3;
	mess_4 m_m4;
	mess_5 m_m5;
	mess_6 m_m6;
  } m_u;
} message;

#define MESS_SIZE (sizeof(message))
#define NIL_MESS (message *) 0

/* The following defines provide names for useful members. */
#define m1_i1	m_u.m_m1.m1i1
#define m1_i2	m_u.m_m1.m1i2
#define m1_i3	m_u.m_m1.m1i3
#define m1_p1	m_u.m_m1.m1p1
#define m1_p2	m_u.m_m1.m1p2
#define m1_p3	m_u.m_m1.m1p3

#define m2_i1	m_u.m_m2.m2i1
#define m2_i2	m_u.m_m2.m2i2
#define m2_i3	m_u.m_m2.m2i3
#define m2_l1	m_u.m_m2.m2l1
#define m2_l2	m_u.m_m2.m2l2
#define m2_p1	m_u.m_m2.m2p1

#define m3_i1	m_u.m_m3.m3i1
#define m3_i2	m_u.m_m3.m3i2
#define m3_p1	m_u.m_m3.m3p1
#define m3_ca1	m_u.m_m3.m3ca1


#define m4_l1	m_u.m_m4.m4l1
#define m4_l2	m_u.m_m4.m4l2
#define m4_l3	m_u.m_m4.m4l3
#define m4_l4	m_u.m_m4.m4l4

#define m5_c1	m_u.m_m5.m5c1
#define m5_c2	m_u.m_m5.m5c2
#define m5_i1	m_u.m_m5.m5i1
#define m5_i2	m_u.m_m5.m5i2
#define m5_l1	m_u.m_m5.m5l1
#define m5_l2	m_u.m_m5.m5l2
#define m5_l3	m_u.m_m5.m5l3

#define m6_i1	m_u.m_m6.m6i1
#define m6_i2	m_u.m_m6.m6i2
#define m6_i3	m_u.m_m6.m6i3
#define m6_l1	m_u.m_m6.m6l1
#define m6_f1	m_u.m_m6.m6f1

struct mem_map {
  vir_clicks mem_vir;		/* virtual address */
  phys_clicks mem_phys;		/* physical address */
  vir_clicks mem_len;		/* length */
};

struct copy_info {		/* used by sys_copy(src, dst, bytes) */
  int cp_src_proc;
  int cp_src_space;
  vir_bytes cp_src_vir;
  int cp_dst_proc;
  int cp_dst_space;
  vir_bytes cp_dst_vir;
  vir_bytes cp_bytes;
};

struct iorequest_s {
  long io_position;		/* position in device file (really off_t) */
  char *io_buf;			/* buffer in user space */
  unsigned short io_nbytes;	/* size of request */
  unsigned short io_request;	/* read, write (optionally) */
};
#endif /* _TYPE_H */
