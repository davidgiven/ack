/* $Header$ */

struct f_info {
	unsigned int f_lineno;
	char *f_filename;
	char *f_workingdir;
};

extern struct f_info file_info;
#define LineNumber file_info.f_lineno
#define FileName file_info.f_filename
