/* The global datastructures (see "data.c"). */

#define cur_value	_cur_value
#define get4		_get4
#define cur_seg		_cur_seg
#define	text		_text
#define data		_data
#define string		_string
#define nname		_nname
#define nbss		_nbss
#define size_text	_size_text
#define size_data	_size_data
#define size_reloc	_size_reloc
#define size_symbol	_size_symbol
#define size_string	_size_string
#define text_cnt	_text_cnt
#define data_cnt	_data_cnt
#define text_area	_text_area
#define data_area	_data_area
#define string_area	_string_area
#define	reloc_info	_reloc_info
#define relo		_relo
#define symbol_table	_symbol_table
#define mem_text	_mem_text
#define mem_data	_mem_data
#define mem_relo	_mem_relo
#define mem_string	_mem_string
#define mem_symbol_hash	_mem_symbol_hash
#define put1		_put1
#define put2		_put2
#define put4		_put4
#define get1		_get1
#define get2		_get2
#define get4		_get4
#define do_local_relocation	_do_local_relocation
#define string_lengte		_string_length
#define index_symbol_table	_index_symbol_table
#define output_back	_output_back

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

#define _put1(buf, off, w)	((buf)[off] = (w))
#define _get1(buf, off)		((buf)[off])
