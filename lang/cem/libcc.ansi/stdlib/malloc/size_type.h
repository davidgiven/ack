#if	_EM_WSIZE == _EM_PSIZE
typedef unsigned int size_type;
#elif	_EM_LSIZE == _EM_PSIZE
typedef unsigned long size_type;
#else
#error funny pointer size
#endif
#include	<stdlib.h>
