#define	CL_DEF		(1<<0)
#define	CL_DECL		(1<<1)
#define	CL_USAGE	(1<<2)

#define	CL_FUNC		(1<<3)
#define	CL_VAR		(1<<4)

#define	CL_LIB		(1<<5)
#define	CL_EXT		(1<<6)
#define	CL_IMPL		(1<<7)
#define	CL_STAT		(1<<8)

extern int class[];

#define	is_class(id,cl)	((class[(id)->id_class-'a'] & (cl)) == (cl))

