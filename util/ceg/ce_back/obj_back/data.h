/* The global datastructures (see "data.c"). */

#define cur_value	B_cur_value
#define get4		B_get4
#define cur_seg		B_cur_seg
#define	text		B_text
#define data		B_data
#define string		B_string
#define nname		B_nname
#define nbss		B_nbss
#define size_text	B_size_text
#define size_data	B_size_data
#define size_reloc	B_size_reloc
#define size_symbol	B_size_symbol
#define size_string	B_size_string
#define text_cnt	B_text_cnt
#define data_cnt	B_data_cnt
#define text_area	B_text_area
#define data_area	B_data_area
#define string_area	B_string_area
#define	reloc_info	B_reloc_info
#define relo		B_relo
#define symbol_table	B_symbol_table
#define mem_text	B_mm_text
#define mem_data	B_mm_data
#define mem_relo	B_mm_relo
#define mem_string	B_mm_string
#define mem_symbol_hash	B_mm_symbol_hash
#define put1		B_put1
#define put2		B_put2
#define put4		B_put4
#define get1		B_get1
#define get2		B_get2
#define get4		B_get4
#define do_local_relocation	B_do_local_relocation
#define string_lengte		B_string_length
#define index_symbol_table	B_index_symbol_table
#define output_back	B_output_back
#define find_sym	B_find_sym
#define Label		B_lab
#define Label_waiting	B_lab_waiting

extern long 	cur_value();
extern long	get4();

extern int 	cur_seg;

extern char 	*text, *data, *string;
extern int	nname;
extern long	nbss, size_text, size_data, size_reloc, size_symbol,
		size_string, text_cnt, data_cnt;

extern char             *text_area, *data_area, *string_area;
extern struct outrelo	*reloc_info, *relo;
extern struct outname	*symbol_table;

#define B_put1(buf, off, w)	((buf)[off] = (w))
#define B_get1(buf, off)		((buf)[off])
