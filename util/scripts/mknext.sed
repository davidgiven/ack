s/^.*[ 	]ALLOCDEF[ 	].*"\(.*\)".*$/struct \1 *h_\1 = 0;\
#ifdef DEBUG\
int cnt_\1 = 0;\
#endif/p
