#include	<pc_file.h>

extern		_wstrin();

_wsi(w,i,f) int w,i; struct file *f; {
	char *p; int j; char buf[6];

	p = &buf[6];
	if ((j=i) < 0) {
		if (i == -32768) {
			_wstrin(w,6,"-32768",f);
			return;
		}
		j = -j;
	}
	do
		*--p = '0' + j%10;
	while (j /= 10);
	if (i<0)
		*--p = '-';
	_wstrin(w,&buf[6]-p,p,f);
}

_wri(i,f) int i; struct file *f; {
	_wsi(6,i,f);
}
