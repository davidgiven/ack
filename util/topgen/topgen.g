/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* t o p g e n . g
 *
 * Grammar of optimizer description, and some code generation
 */

%token	LETTER, DIGIT, OTHER, SPACE;
%token	LINE_TERMINATOR, OPERAND_SEPARATOR, INSTRUCTION_SEPARATOR,
	PATTERN_SEPARATOR, OPEN_BRACKET, CLOSE_BRACKET;
%lexical LLlex;
%start	LLparse, optim_description;

{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"
#include "symtab.h"
#include "misc.h"

char	idbuf[BUFSIZ], buf[BUFSIZ];
int	countid;		/* # of variables */
int	countpat;		/* # of patterns */
static int patlen;		/* Maximum number of instructions in pattern */
static int maxoperand;		/* Maximum number of operands of instruction */
extern FILE *input;		/* file descriptor of inputfile */
}

optim_description
	{ struct symtab *p; } :
	SPACE* parameter_line*
			{   p = findident("MAXOP",LOOKING,&deftable);
			    if (p == 0) maxoperand = 2;	/* default */
			    else maxoperand = p->s_num;
			}
	separator SPACE* mode_definitions
	separator SPACE* patterns		
	separator 
			{   register int c;
			    fprintf(genc, linedir, lineno, inpfile);
			    while ((c = getc(input)) != EOF) {
				putc(c,genc);
			    }
			}
;

parameter_line
	{ struct symtab *p;} :
	identifier
			{   p = findident(idbuf,ENTERING,&deftable);}
	SPACE
	value
			{   p->s_num = atoi(buf);}
			/* This action in fact only needed for MAXOP */
	LINE_TERMINATOR
	SPACE*
			{   fprintf(genh,"#define %s %s\n",p->s_name,buf);}
;

value
	{ char *p1 = buf;} :
	[
	    [	OPEN_BRACKET
	    |	CLOSE_BRACKET
	    |	OPERAND_SEPARATOR
	    |   PATTERN_SEPARATOR
	    |   INSTRUCTION_SEPARATOR
	    |	SPACE
	    |	LETTER
	    |	DIGIT
	    |	OTHER
	    |	'%'
	    ]
			{   *p1++ = dot.t_attrib;}
	]*
			{   *p1 = '\0';}
;

mode_definitions
	{ int lin; } :
			{   fputs("tok_chk(varno) {\n\tint r;\n", genc);
			    fputs("\tchar *VAL;\n\n",genc);
			    fputs("\tVAL = var[varno].value;\n",genc);
			    fputs("\tswitch(varno) {\n",genc);
			}
	[
	    token_list
	    constraint(&lin)
			{   fprintf(genc,linedir,lin,inpfile);
			    fprintf(genc,"\t\tr = (%s); break;\n",buf);
			}
	    LINE_TERMINATOR
	    SPACE*
	]*
			{   fputs("\tdefault :\n\t\tassert(0);\n",genc);
			    fputs("\t}\n\treturn r;\n}\n\n",genc);
			}
;

token_list :
	new_identifier
	SPACE*
	[
	    OPERAND_SEPARATOR
	    SPACE*
	    new_identifier
	    SPACE*
	]*
;

new_identifier
	{ struct symtab *p;} :
	identifier
			{   p = findident(idbuf,ENTERING,&idtable);
			    p->s_num = ++countid;
			    fprintf(genc,"\tcase %d:\n", countid);
			}
;

constraint (int *lin;)
	{ char *p = buf; } :
	OPEN_BRACKET
			{   *lin = lineno;}
	[
	    [	LINE_TERMINATOR
	    |	OPERAND_SEPARATOR
	    |	PATTERN_SEPARATOR
	    |	INSTRUCTION_SEPARATOR
	    |	LETTER
	    |	DIGIT
	    |	SPACE
	    |	OTHER
	    |	'%'
	    ]
			{   *p++ = dot.t_attrib;}
	]*
			{   *p = '\0';
			    if (onlyspace(buf)) strcpy(buf,"TRUE");
			}
	CLOSE_BRACKET
	SPACE*
;

patterns
	{ int lin;
	  char *constr;  
	  int np, nr;
	} :
[
			{   countpat++;
			    constr = (char *) 0;
			    fprintf(genc,"struct instr_descr pat%d[] = {\n",
					 countpat);
			}
	instruction_list(&np)
			{   if (np > patlen) patlen = np;
			    fputs("\n};\n\n",genc);
			}
	[
	    constraint(&lin)
	   		{   /* Save the constraint, we need it later on */
			    constr = malloc((unsigned)(strlen(buf)+1));
			    strcpy(constr,buf);
			}
	]?
	PATTERN_SEPARATOR
			{   fprintf(genc,"struct instr_descr rep%d[] = {\n",
					 countpat);
			}
	replacement(&nr)
			{   fputs("\n};\n\n",genc);}
	LINE_TERMINATOR
	SPACE*
			{   addpattern(constr,lin,np,nr);}
]*
			{   printhashtable();
			    printpatterns();
			    fprintf(genh,"#define NRVARS %d\n",countid);
			    fprintf(genh,"#define NRPATTERNS %d\n",countpat);
			    fprintf(genh,"#define MIN_WINDOW_SIZE %d\n",
					patlen+3);
			    fclose(genh);
			}
;

instruction_list(int *n;) :
	instruction(1)
			{   *n = 1;}
	[
	    INSTRUCTION_SEPARATOR
	    		{   fputs(",\n",genc);}
	    SPACE*
	    instruction(0)
	    		{   *n += 1;}
	]*
;

instruction(int opt;)
	{ int count = 0;} :
	opcode(opt)
			{   if (strcmp(buf,"ANY") != 0) {
			        fprintf(genc,"\t{\"%s\", {",buf);
			    }
			    else fputs("\t{(char *) 0, {",genc);
			    count = 0;
			}
	[
	    operand(' ')
	    		{   count = 1;}
	    [
	    	OPERAND_SEPARATOR
			{   count++;}
	    	SPACE*
	    	operand(',')
	    ]*
	    		{   if (count > maxoperand) {
			        error("Too many operands");
			    }
			}
	]?
			{   while (count++ < maxoperand) {
				fprintf(genc,"%c{\"\",-1,\"\"}",count == 1 ? ' ' : ',');
			    }
			    putc('}',genc);
			    putc('}',genc);
			}
;

opcode(int opt;)
	{ char *p = buf;} :
	[
	    [	LETTER
	    |	DIGIT
	    |	OTHER
	    ]
			{   *p++ = dot.t_attrib;}
	]+
	SPACE+
			{   *p = '\0';
			    if (opt) addtohashtable(buf,countpat);
			}
;

operand(int c;)
	{ register struct symtab *p = 0;} :
			{   fprintf(genc, "%c{\"", c);}
	[
	    identifier
	    		{   if (!p) {
				p = findident(idbuf,LOOKING,&idtable);
				if (p) fprintf(genc,"\",%d,\"",p->s_num);
				else fputs(idbuf,genc);
			    }
			    else fputs(idbuf,genc);
			}
	|   DIGIT
	    		{   putc(dot.t_attrib,genc);}
	|   OTHER
	    		{   putc(dot.t_attrib,genc);}
	]+
			{   if (p) fputs("\"}",genc);
			    else fputs("\",0,\"\"}",genc);
			}
	SPACE*
;

replacement (int *n;)
	{register i;} :
	SPACE*
			{   *n = 0;}
	[
	    instruction(0)
			{   *n = 1;}
	    [
	    	INSTRUCTION_SEPARATOR
	    		{   fputs(",\n", genc);}
	    	SPACE*
	    	instruction(0)
	    		{   *n += 1;}
	    ]*
	|		/* empty replacement, but there must be a
			 * structure initializer anyway
			 */
			{   fputs("\t{\"\", {",genc);
			    for (i = 0; i < maxoperand; i++) {
				fprintf(genc, "%c{\"\",-1,\"\"}",i?',':' ');
			    }
			    fputs("}}",genc);
			}
	]
;


identifier
	{ char *p = idbuf; } :
	LETTER
				{ *p++ = dot.t_attrib;}
	[ %while (1)
	    LETTER		{ *p++ = dot.t_attrib;}
	|   DIGIT		{ *p++ = dot.t_attrib;}
	]*
				{ *p = '\0';}
;

separator :
	'%' '%' SPACE* LINE_TERMINATOR
;
