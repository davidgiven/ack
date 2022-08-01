#include <em_abs.h>

char* _trpstr(int d)
{
	switch(d)
	{
	case EARRAY: return "array bound error";
	case ERANGE: return "range bound error";
	case ESET: return "set bound error";
	case EIOVFL: return "integer overflow";
	case EFOVFL: return "floating overflow";
	case EFUNFL: return "floating underflow";
	case EIDIVZ: return "divide by 0";
	case EFDIVZ: return "divide by 0.0";
	case EIUND: return "undefined integer";
	case EFUND: return "undefined real";
	case ECONV: return "conversion error";
	case ESTACK: return "stack overflow";
	case EHEAP: return "heap overflow";
	case EILLINS: return "illegal instruction";
	case EODDZ: return "illegal size argument";
	case ECASE: return "case error";
	case EMEMFLT: return "addressing non-existent memory";
	case EBADPTR: return "bad pointer used";
	case EBADPC: return "program counter out of range";
	case EBADMON: return "bad monitor call";
	case EBADLIN:
	case EBADGTO:
	case EBADLAE:
	default:
		return "strange trap";
	}
	/*NOTREACHED*/
}
