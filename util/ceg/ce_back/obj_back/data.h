/* The global datastructures (see "data.c"). */

extern long 	cur_value();
extern long	get4();

extern int 	cur_seg;

extern char 	*text, *data, *string;
extern int	nname;
extern long	nbss, size_text, size_data, size_reloc, size_symbol,
		size_string, _text_cnt, _data_cnt;

extern char             *text_area, *data_area, *string_area;
extern struct outrelo	*reloc_info, *relo;
extern struct outname	*symbol_table;

#define put1(buf, off, w)	((buf)[off] = (w))
#define get1(buf, off)		((buf)[off])
