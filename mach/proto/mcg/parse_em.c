#include "mcg.h"

static struct e_instr em;
static struct procedure* current_proc;
static struct basicblock* code_bb;
static struct basicblock* data_bb;

static void queue_insn_label(int opcode, const char* label, arith offset);

static const char* type_to_str(int type)
{
	switch (type)
	{
		case EM_MNEM:       return "EM_MNEM";
		case EM_PSEU:	    return "EM_PSEU";
		case EM_STARTMES:   return "EM_STARTMES";
		case EM_MESARG:     return "EM_MESARG";
		case EM_ENDMES:     return "EM_ENDMES";
		case EM_DEFILB:     return "EM_DEFILB";
		case EM_DEFDLB:     return "EM_DEFDLB";
		case EM_DEFDNAM:    return "EM_DEFDNAM";
		case EM_ERROR:      return "EM_ERROR";
		case EM_FATAL:      return "EM_FATAL";
		case EM_EOF:        return "EM_EOF";
	}

    assert(0 && "invalid EM type");
}

static const char* argtype_to_str(int type)
{
    if (type == 0)        return "...";
    if (type == ilb_ptyp) return "ilb";
    if (type == nof_ptyp) return "nof";
    if (type == sof_ptyp) return "sof";
    if (type == cst_ptyp) return "cst";
    if (type == pro_ptyp) return "pro";
    if (type == str_ptyp) return "str";
    if (type == ico_ptyp) return "ico";
    if (type == uco_ptyp) return "uco";
    if (type == fco_ptyp) return "fco";
    return "???";
}

static void unknown_type(const char* s)
{
    fatal("%s with unknown type '%s'",
        s,
        argtype_to_str(em.em_arg.ema_argtype));
}

static const char* ilabel_to_str(label l)
{
    assert(current_proc != NULL);
    return aprintf("__%s_I%d", current_proc->name, l);
}

static const char* dlabel_to_str(label l)
{
    return aprintf("__D%d", l);
}

static void terminate_block(void)
{
    code_bb->is_terminated = true;
    code_bb = NULL;
}

static struct em* new_insn(int opcode)
{
    struct em* em = calloc(sizeof(struct em), 1);
    em->opcode = opcode;
    return em;
}

static void queue_insn_simple(int opcode)
{
    struct em* em = new_insn(opcode);
    em->paramtype = PARAM_NONE;
    array_append(&code_bb->ems, em);

    switch (opcode)
    {
        case op_bra:
            terminate_block();
            break;
    }
}

static void queue_insn_value(int opcode, arith value)
{
    struct em* em = new_insn(opcode);
    em->paramtype = PARAM_IVALUE;
    em->u.ivalue = value;
    array_append(&code_bb->ems, em);

    switch (opcode)
    {
        case op_csa:
        case op_csb:
        case op_ret:
            terminate_block();
            break;
    }
}

static void queue_insn_label(int opcode, const char* label, arith offset)
{
    struct em* em = new_insn(opcode);
    em->paramtype = PARAM_LVALUE;
    em->u.lvalue.label = label;
    em->u.lvalue.offset = offset;
    array_append(&code_bb->ems, em);

    switch (opcode)
    {
        case op_bra:
            terminate_block();
            break;
    }
}

static void queue_insn_block(int opcode, struct basicblock* left, struct basicblock* right)
{
    struct em* em = new_insn(opcode);
    em->paramtype = PARAM_BVALUE;
    em->u.bvalue.left = left;
    em->u.bvalue.right = right;
    array_append(&code_bb->ems, em);
    
    terminate_block();
}

static void change_basicblock(struct basicblock* newbb)
{
    array_appendu(&current_proc->blocks, newbb);

    if (code_bb && !code_bb->is_terminated)
        queue_insn_block(op_bra, newbb, NULL);

    code_bb = newbb;
}

static void queue_insn_ilabel(int opcode, int label)
{
    const char* name = ilabel_to_str(em.em_ilb);
    struct basicblock* left = bb_get(name);

    switch (opcode)
    {
        case op_bra:
            queue_insn_block(em.em_opcode, left, NULL);
            break;

        case op_zeq:
        case op_zne:
        case op_zlt:
        case op_zle:
        case op_zgt:
        case op_zge:
        case op_beq:
        case op_bne:
        case op_blt:
        case op_ble:
        case op_bgt:
        case op_bge:
        {
            struct basicblock* bb = bb_get(NULL);
            queue_insn_block(em.em_opcode, left, bb);
            change_basicblock(bb);
            break;
        }

        default:
            fatal("parse_em: unhandled conditional '%s'", 
                em_mnem[opcode - sp_fmnem]);
    }
}

static void queue_ilabel(arith label)
{
    change_basicblock(bb_get(ilabel_to_str(label)));
}

static void parse_pseu(void)
{
	switch (em.em_opcode)
	{
		case ps_exp: /* external proc */
		case ps_exa: /* external array */
		case ps_inp: /* internal proc */
		case ps_ina: /* internal array */
		{
			bool export = (em.em_opcode == ps_exp) || (em.em_opcode == ps_exa);
			bool proc = (em.em_opcode == ps_exp) || (em.em_opcode == ps_inp);

			switch (em.em_arg.ema_argtype)
			{
				case pro_ptyp:
					symbol_declare(strdup(em.em_pnam), export, proc);
					break;

				case sof_ptyp:
                    assert(em.em_off == 0);
					symbol_declare(strdup(em.em_dnam), export, proc);
					break;

                case nof_ptyp:
                    assert(em.em_off == 0);
                    symbol_declare(dlabel_to_str(em.em_dlb), export, proc);
                    break;

				default:
                    unknown_type("exp, exa, inp, ina");
			}
			break;
		}

		case ps_con: /* .data */
		case ps_rom: /* .rom */
        {
            bool ro = (em.em_opcode == ps_rom);

			switch (em.em_arg.ema_argtype)
			{
				case ico_ptyp:
				case uco_ptyp:
                {
                    arith val = atol(em.em_string);
                    data_int(val, em.em_size, ro);
                    break;
                }

				case str_ptyp:
                    data_block(strdup(em.em_string), em.em_size, ro);
					break;

                case cst_ptyp:
                    data_int(em.em_cst, EM_wordsize, ro);
                    break;
                    
                case nof_ptyp:
                    data_offset(dlabel_to_str(em.em_dlb), em.em_off, ro);
                    break;

                case ilb_ptyp:
                {
                    const char* label = ilabel_to_str(em.em_ilb);

                    /* This is really hacky; to handle basic block flow
                     * descriptor blocks, we need to track which bbs a descriptor
                     * can exit to. So we create fake bb objects for each
                     * block, purely to track this.
                     */

                    if (data_bb)
                    {
                        struct em* em = new_insn(op_bra);
                        em->paramtype = PARAM_BVALUE;
                        em->u.bvalue.left = bb_get(label);
                        array_append(&data_bb->ems, em);
                    }

                    data_offset(label, 0, ro);
                    break;
                }

				default:
                    unknown_type("con, rom");
			}
			break;
        }

        case ps_bss:
        {
            switch (em.em_arg.ema_argtype)
            {
                case cst_ptyp:
                    data_bss(EM_bsssize, em.em_cst);
                    break;
                    
                default:
                    unknown_type("bss");
            }
            break;
        }

		case ps_pro: /* procedure start */
        {
            struct symbol* symbol;

            current_proc = calloc(sizeof(struct procedure), 1);
            current_proc->name = strdup(em.em_pnam);
            current_proc->root_bb = bb_get(current_proc->name);
            current_proc->nlocals = em.em_nlocals;
            code_bb = current_proc->root_bb;
            code_bb->is_root = true;
            array_append(&current_proc->blocks, code_bb);

            symbol = symbol_get(current_proc->name);
            symbol->section = SECTION_TEXT;
            symbol->proc = current_proc;
            symbol->is_proc = true;
            break;
        }

		case ps_end: /* procedure end */
            tb_procedure(current_proc);

            current_proc = NULL;
            code_bb = NULL;
			break;

		default:
            fatal("unknown pseudo with opcode %d\n", em.em_opcode);
	}
}

static arith mes_get_cst(void)
{
    EM_getinstr(&em);
    if (em.em_type != EM_MESARG)
        fatal("malformed MES");
    return em.em_cst;
}

static void parse_mes(void)
{
    assert(em.em_arg.ema_argtype == cst_ptyp);
    switch (em.em_cst)
    {
        case 0: /* error */
            fatal("MES 0 received (explicit halt)");

        case 3: /* register variable */
        {
            /* ego will sometimes generate 'mes 3' pseudos with no actual
             * parameters. Detect and ignore these. */
            
            EM_getinstr(&em);
            if (em.em_type == EM_MESARG)
            {
                arith offset = em.em_cst;
                int size = mes_get_cst();
                int type = mes_get_cst();
                int priority = mes_get_cst();
                tb_regvar(current_proc, offset, size, type, priority);
            }
            break;
        }
    }

    while ((em.em_type == EM_STARTMES) || (em.em_type == EM_MESARG))
        EM_getinstr(&em);

    if (em.em_type != EM_ENDMES)
        fatal("malformed MES");
}

static void create_data_label(const char* label)
{
    data_label(label);
    if (current_proc)
    {
        data_bb = bb_get(label);
        data_bb->is_fake = true;
        array_append(&current_proc->blocks, data_bb);
    }
}

void parse_em(void)
{
    EM_getinstr(&em);
	tb_filestart();

	while (em.em_type != EM_EOF)
	{
        switch (em.em_type)
        {
            case EM_PSEU:
				parse_pseu();
                break;

            case EM_DEFILB:
                queue_ilabel(em.em_ilb);
                break;

            case EM_DEFDLB:
                create_data_label(dlabel_to_str(em.em_dlb));
                break;

            case EM_DEFDNAM:
                create_data_label(strdup(em.em_dnam));
                break;

            case EM_STARTMES:
                parse_mes();
                break;

            case EM_MNEM:
                if (code_bb)
                {
                    int flags = em_flag[em.em_opcode - sp_fmnem];

                    if (flags & EM_PAR)
                    {
                        switch (em.em_argtype)
                        {
                            case ilb_ptyp:
                                queue_insn_ilabel(em.em_opcode, em.em_ilb);
                                break;

                            case nof_ptyp:
                                queue_insn_label(em.em_opcode,
                                    dlabel_to_str(em.em_dlb), em.em_off);
                                break;

                            case sof_ptyp:
                                queue_insn_label(em.em_opcode,
                                    strdup(em.em_dnam), em.em_off);
                                break;

                            case pro_ptyp:
                                queue_insn_label(em.em_opcode,
                                    strdup(em.em_pnam), 0);
                                break;

                            case cst_ptyp:
                                if ((flags & EM_PAR) == PAR_B)
                                    queue_insn_ilabel(em.em_opcode, em.em_ilb);
                                else
                                    queue_insn_value(em.em_opcode, em.em_cst);
                                break;

                            default:
                                unknown_type("instruction");
                        }
                    }
                    else
                        queue_insn_simple(em.em_opcode);
                }
                break;

            default:
                fatal("unrecognised instruction type '%d'", em.em_type);
        }

		EM_getinstr(&em);
	}

	tb_fileend();
}

/* vim: set sw=4 ts=4 expandtab : */
