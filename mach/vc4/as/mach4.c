/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

operation
	: OP                                              { emit2($1); }

	| OP_BRANCH GPR                        { emit2($1 | ($2<<0)); }
	| OP_BRANCH expr                       { branch_instr($1, ALWAYS, &$2); }
	| OP_BRANCH CC expr                    { branch_instr($1, $2, &$3); }

	| OP_ONELREG GPR
	{
		if ($2 >= 0x10)
			serror("cannot use r16+ here");
		emit2($1 | ($2<<0));
	}

	| OP_ALU GPR ',' GPR                   { alu_instr_reg($1, ALWAYS, $2, $2, $4); }
	| OP_ALU GPR ',' GPR ',' GPR           { alu_instr_reg($1, ALWAYS, $2, $4, $6); }
	| OP_ALU CC GPR ',' GPR                { alu_instr_reg($1, $2, $3, $3, $5); }
	| OP_ALU CC GPR ',' GPR ',' GPR        { alu_instr_reg($1, $2, $3, $5, $7); }

	| OP_ALU GPR ',' '#' absexp            { alu_instr_lit($1, ALWAYS, $2, $2, $5); }
	| OP_ALU GPR ',' GPR ',' '#' absexp    { alu_instr_lit($1, ALWAYS, $2, $4, $7); }
	| OP_ALU CC GPR ',' '#' absexp         { alu_instr_lit($1, $2, $3, $3, $6); }
	| OP_ALU CC GPR ',' GPR ',' '#' absexp { alu_instr_lit($1, $2, $3, $5, $8); }

	| OP_MISC GPR ',' GPR ',' GPR          { misc_instr_reg($1, ALWAYS, $2, $4, $6); }
	| OP_MISC CC GPR ',' GPR ',' GPR       { misc_instr_reg($1, $2, $3, $5, $7); }

	| OP_MISCL GPR ',' GPR ',' GPR         { misc_instr_reg($1, ALWAYS, $2, $4, $6); }
	| OP_MISCL CC GPR ',' GPR ',' GPR      { misc_instr_reg($1, $2, $3, $5, $7); }
	| OP_MISCL GPR ',' GPR ',' '#' absexp  { misc_instr_lit($1, ALWAYS, $2, $4, $7); }
	| OP_MISCL CC GPR ',' GPR ',' '#' absexp { misc_instr_lit($1, $2, $3, $5, $8); }

	| OP_STACK GPR                         { stack_instr($1, $2, $2, -1); }
	| OP_STACK GPR ',' GPR                 { stack_instr($1, $2, $2, $4); }
	| OP_STACK GPR '-' GPR                 { stack_instr($1, $2, $4, -1); }
	| OP_STACK GPR '-' GPR ',' GPR         { stack_instr($1, $2, $4, $6); }

	| OP_MEM GPR ',' '(' GPR ')'           { mem_instr($1, ALWAYS, $2, 0, $5); }
	| OP_MEM CC GPR ',' '(' GPR ')'        { mem_instr($1, $2, $3, 0, $6); }
	| OP_MEM GPR ',' absexp '(' GPR ')'    { mem_instr($1, ALWAYS, $2, $4, $6); }
	| OP_MEM CC GPR ',' absexp '(' GPR ')' { mem_instr($1, $2, $3, $5, $7); }

    | OP_MEM GPR ',' '(' GPR ',' GPR ')'
    | OP_MEM CC GPR ',' '(' GPR ',' GPR ')'

    | OP_MEM GPR ',' '(' GPR ')' '+' '+'
    | OP_MEM CC GPR ',' '(' GPR ')' '+' '+'
	;

