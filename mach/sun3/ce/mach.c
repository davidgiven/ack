#define CODE_EXPANDER
#include "mach.h"
#include <back.h>
#include <system.h>


#ifdef DEBUG
arg_error( s, arg)
char *s;
int arg;
{
	fprint( STDERR, "arg_error %s %d\n", s, arg);
}
#endif


/*
do_open( filename)
char *filename;
{
	if ( filename == (char *)0 || !sys_open( filename, OP_WRITE, &codefile))
		return( 0);

	fprint( codefile, ".sect .text; .sect .rom; .sect .data; .sect .bss\n");
	return( 1);
}
*/

#include <con_float>

__instr_code(code, reg, off)
{
  if (off <= 32767 & off >= -32768) {
	text2(code|0x28|reg);
	text2(off);
	return;
  }
  text2(code|0x30|reg);
  text2(0x0170);
  text4(off);
}

__move_X(code, reg, off)
{
  if (off <= 32767 & off >= -32768) {
	text2(code|(reg<<9)|0x140);
	text2(off);
	return;
  }
  text2(code|(reg<<9)|0x180);
  text2(0x0170);
  text4(off);
}

__moveXX(code, srcreg, srcoff, dstreg, dstoff)
{
  if (srcoff <= 32767 && srcoff >= -32768) {
	__move_X(code|0x28|srcreg, dstreg, dstoff);
	return;
  }
  if (dstoff <= 32767 && dstoff >= -32768) {
	__instr_code(code|0x140|(dstreg<<9), srcreg, srcoff);
	return;
  }
  text2(code|(dstreg<<9)|srcreg|0x180|0x30);
  text2(0x0170);
  text4(srcoff);
  text2(0x0170);
  text4(dstoff);
}
