extern long 	cur_value();

extern int 	cur_seg;

extern char 	*text, *data, *string;
extern int	nname;
extern long	nbss, size_text, size_data, size_reloc, size_symbol,
		size_string;

extern char             *text_area, *data_area, *string_area;
extern struct outrelo	*reloc_info, *relo;
extern struct outname	*symbol_table;

