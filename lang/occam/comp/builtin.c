/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "symtab.h"
#include "expr.h"
#include "sizes.h"

void init_builtins()
/* Insert all builtin names into the outermost symbol table (first statement
 * is sym_down() ).  Note that this table is never destroy()ed, so static
 * initializers may be used.
 */
{
	union type_info info;

	static char file[]="file";

	static struct par_list
	open_list[] = {
		{ &open_list[1], nil, T_VAR },		/* File descriptor */
		{ &open_list[2], nil, T_VALUE|T_ARR },	/* File name */
		{ nil,		 nil, T_VALUE|T_ARR }	/* "r", "w", "a" */
	},
	close_list[]= {
		{ nil,		 nil, T_VALUE }		/* File descriptor */
	},
	exit_list[]= {
		{ nil,		 nil, T_VALUE }		/* Exit code */
	};

	sym_down();	/* Add level of symbols above all others */

	/* CHAN file[20], input=file[0], output=file[1], error=file[2]: */

	info.vc.st.builtin=file;
	info.vc.offset=0;
	insert(file, T_CHAN|T_ARR|T_BUILTIN, 20, &info);

	info.vc.st.builtin=file;
	info.vc.offset=0;
	insert("input", T_CHAN|T_BUILTIN, 1, &info);

	info.vc.st.builtin=file;
	info.vc.offset=wz+vz;
	insert("output", T_CHAN|T_BUILTIN, 1, &info);

	info.vc.st.builtin=file;
	info.vc.offset=2*(wz+vz);
	insert("error", T_CHAN|T_BUILTIN, 1, &info);

	/* DEF EOF= -1, TEXT= -2, RAW= -3: */

	info.t_const=new_const(-1L);
	insert("EOF", T_CONST|T_BUILTIN, 0, &info);

	info.t_const=new_const(-2L);
	insert("TEXT", T_CONST|T_BUILTIN, 0, &info);

	info.t_const=new_const(-3L);
	insert("RAW", T_CONST|T_BUILTIN, 0, &info);

	/* PROC open(VAR fd, VALUE name[], mode[])= .... : */
	info.proc.st.builtin="b_open";
	info.proc.pars=open_list;
	insert("open", T_PROC|T_BUILTIN, 0, &info);

	/* PROC close(VALUE fd)= .... : */
	info.proc.st.builtin="b_close";
	info.proc.pars=close_list;
	insert("close", T_PROC|T_BUILTIN, 0, &info);

	/* PROC exit(VALUE code)= .... : */
	info.proc.st.builtin="b_exit";
	info.proc.pars=exit_list;
	insert("exit", T_PROC|T_BUILTIN, 0, &info);
}
