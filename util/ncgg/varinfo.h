/* $Header$ */

#define VI_NSTR 1
#define VI_NINT 3

typedef struct varinfo {
	struct varinfo *vi_next;
	char *vi_str[VI_NSTR];
	int vi_int[VI_NINT];
	struct varinfo *vi_vi;
} varinfo;

#define VI_NULL (struct varinfo *) 0
