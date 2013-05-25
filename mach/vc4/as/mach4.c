/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

operation
	: OP                                   { emit2($1); }

	| OP_BRANCH GPR                        { emit2(B16(00000000,01000000) | ($2<<0)); }
	| OP_BRANCHLINK GPR                    { emit2(B16(00000000,01100000) | ($2<<0)); }

	| OP_BRANCH expr                       { branch_instr(0, ALWAYS, &$2); }
	| OP_BRANCHLINK expr                   { branch_instr(1, ALWAYS, &$2); }
	| OP_BRANCH CC expr                    { branch_instr(0, $2, &$3); }
	| OP_BRANCHLINK CC expr                { branch_instr(1, $2, &$3); }

	| OP_BRANCH GPR ',' GPR ',' expr       { branch_addcmp_lit_reg_instr(ALWAYS, $2, 0, $4, &$6); }
	| OP_BRANCH CC GPR ',' GPR ',' expr    { branch_addcmp_lit_reg_instr($2, $3, 0, $5, &$7); }
	| OP_BRANCH GPR ',' '#' absexp ',' expr { branch_addcmp_lit_lit_instr(ALWAYS, $2, 0, $5, &$7); }
	| OP_BRANCH CC GPR ',' '#' absexp ',' expr { branch_addcmp_lit_lit_instr($2, $3, 0, $6, &$8); }
	| OP_ADDCMPB GPR ',' GPR ',' GPR ',' expr { branch_addcmp_reg_reg_instr(ALWAYS, $2, $4, $6, &$8); }
	| OP_ADDCMPB CC GPR ',' GPR ',' GPR ',' expr { branch_addcmp_reg_reg_instr($2, $3, $5, $7, &$9); }
	| OP_ADDCMPB GPR ',' '#' absexp ',' GPR ',' expr { branch_addcmp_lit_reg_instr(ALWAYS, $2, $5, $7, &$9); }
	| OP_ADDCMPB CC GPR ',' '#' absexp ',' GPR ',' expr { branch_addcmp_lit_reg_instr($2, $3, $6, $8, &$10); }
	| OP_ADDCMPB GPR ',' GPR ',' '#' absexp ',' expr { branch_addcmp_reg_lit_instr(ALWAYS, $2, $4, $7, &$9); }
	| OP_ADDCMPB CC GPR ',' GPR ',' '#' absexp ',' expr { branch_addcmp_reg_lit_instr($2, $3, $5, $8, &$10); }
	| OP_ADDCMPB GPR ',' '#' absexp ',' '#' absexp ',' expr { branch_addcmp_lit_lit_instr(ALWAYS, $2, $5, $8, &$10); }
	| OP_ADDCMPB CC GPR ',' '#' absexp ',' '#' absexp ',' expr { branch_addcmp_lit_lit_instr($2, $3, $6, $9, &$11); }

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

    | OP_MEM GPR ',' '(' GPR ',' GPR ')'   { mem_offset_instr($1, ALWAYS, $2, $5, $7); }
    | OP_MEM CC GPR ',' '(' GPR ',' GPR ')' { mem_offset_instr($1, $2, $3, $6, $8); }

    | OP_MEM GPR ',' '(' GPR ')' '+' '+'   { mem_postincr_instr($1, ALWAYS, $2, $5); }
    | OP_MEM CC GPR ',' '(' GPR ')' '+' '+' { mem_postincr_instr($1, $2, $3, $6); }

    | OP_MEM GPR ',' expr                  { mem_address_instr($1, $2, &$4); }

    | OP_LEA GPR ',' absexp '(' GPR ')'    { lea_stack_instr($2, $4, $6); }
	| OP_LEA GPR ',' expr                  { lea_address_instr($2, &$4); }
	;

