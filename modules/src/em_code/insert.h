/* Structures used for the C_insertpart, C_beginpart, and C_endpart
   mechanism. Each part consists of a list of chunks. Each chunk is
   either another part, or a piece of text limited by a begin- and an
   end-pointer.
*/

typedef struct partofpart {
	struct partofpart	*pp_next;
	char			pp_type;
#define TEXT	0
#define INSERT	1
	union {
		struct {
			long	ppu_begin, ppu_end;
		} ppu_s;
		int	ppu_id;
	} pp_u;
#define pp_begin	pp_u.ppu_s.ppu_begin
#define pp_end		pp_u.ppu_s.ppu_end
#define pp_id		pp_u.ppu_id
} PartOfPart;

typedef struct part {
	struct part	*p_next;	/* next part in hash chain */
	char		p_flags;
#define BUSY	1
	PartOfPart	*p_parts;	/* chunks of this part */
	struct part	*p_prevpart;	/* implements stack of active parts */
	int		p_id;		/* id of this part */
} Part;

#define		outpart(xxx)		C_out_parts(C_findpart(xxx)->p_parts)

#define TABSIZ	32

extern int
	C_ontmpfile, C_sequential;
extern Part
	*C_curr_part, *C_stable[];
extern long
	C_current_out;
extern Part
	*C_findpart();
