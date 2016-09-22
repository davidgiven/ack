#include "mcg.h"

static struct e_instr insn;
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
        argtype_to_str(insn.em_arg.ema_argtype));
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

static struct insn* new_insn(int opcode)
{
    struct insn* insn = calloc(sizeof(struct insn), 1);
    insn->opcode = opcode;
    return insn;
}

static void queue_insn_simple(int opcode)
{
    struct insn* insn = new_insn(opcode);
    insn->paramtype = PARAM_NONE;
    APPEND(code_bb->insns, insn);

    switch (opcode)
    {
        case op_bra:
            terminate_block();
            break;
    }
}

static void queue_insn_value(int opcode, arith value)
{
    struct insn* insn = new_insn(opcode);
    insn->paramtype = PARAM_IVALUE;
    insn->u.ivalue = value;
    APPEND(code_bb->insns, insn);

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
    struct insn* insn = new_insn(opcode);
    insn->paramtype = PARAM_LVALUE;
    insn->u.lvalue.label = label;
    insn->u.lvalue.offset = offset;
    APPEND(code_bb->insns, insn);

    switch (opcode)
    {
        case op_bra:
            terminate_block();
            break;
    }
}

static void queue_insn_block(int opcode, struct basicblock* left, struct basicblock* right)
{
    struct insn* insn = new_insn(opcode);
    insn->paramtype = PARAM_BVALUE;
    insn->u.bvalue.left = left;
    insn->u.bvalue.right = right;
    APPEND(code_bb->insns, insn);
    
    APPENDU(code_bb->outblocks, left);
    APPENDU(left->inblocks, code_bb);
    if (right)
    {
        APPENDU(code_bb->outblocks, right);
        APPENDU(right->inblocks, code_bb);
    }

    terminate_block();
}

static void change_basicblock(struct basicblock* newbb)
{
    APPENDU(current_proc->blocks, newbb);

    if (code_bb && !code_bb->is_terminated)
        queue_insn_block(op_bra, newbb, NULL);

    code_bb = newbb;
}

static void queue_insn_ilabel(int opcode, int label)
{
    const char* name = ilabel_to_str(insn.em_ilb);
    struct basicblock* left = bb_get(name);

    switch (opcode)
    {
        case op_bra:
            queue_insn_block(insn.em_opcode, left, NULL);
            break;

        case op_zeq:
        case op_zne:
        case op_zlt:
        case op_zle:
        case op_zgt:
        case op_zge:
        {
            struct basicblock* bb = bb_get(NULL);
            queue_insn_block(insn.em_opcode, left, bb);
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
	switch (insn.em_opcode)
	{
		case ps_exp: /* external proc */
		case ps_exa: /* external array */
		case ps_inp: /* internal proc */
		case ps_ina: /* internal array */
		{
			bool export = (insn.em_opcode == ps_exp) || (insn.em_opcode == ps_exa);
			bool proc = (insn.em_opcode == ps_exp) || (insn.em_opcode == ps_inp);

			switch (insn.em_arg.ema_argtype)
			{
				case pro_ptyp:
					symbol_declare(strdup(insn.em_pnam), export, proc);
					break;

				case sof_ptyp:
                    assert(insn.em_off == 0);
					symbol_declare(strdup(insn.em_dnam), export, proc);
					break;

                case nof_ptyp:
                    assert(insn.em_off == 0);
                    symbol_declare(dlabel_to_str(insn.em_dlb), export, proc);
                    break;

				default:
                    unknown_type("exp, exa, inp, ina");
			}
			break;
		}

		case ps_con: /* .data */
		case ps_rom: /* .rom */
        {
            bool ro = (insn.em_opcode == ps_rom);

			switch (insn.em_arg.ema_argtype)
			{
				case ico_ptyp:
				case uco_ptyp:
                {
                    arith val = atol(insn.em_string);
                    data_int(val, insn.em_size, ro);
                    break;
                }

				case str_ptyp:
                    data_block(strdup(insn.em_string), insn.em_size, ro);
					break;

                case cst_ptyp:
                    data_int(insn.em_cst, EM_wordsize, ro);
                    break;
                    
                case nof_ptyp:
                    data_offset(dlabel_to_str(insn.em_dlb), insn.em_off, ro);
                    break;

                case ilb_ptyp:
                {
                    const char* label = ilabel_to_str(insn.em_ilb);

                    /* This is really hacky; to handle basic block flow
                     * descriptor blocks, we need to track which bbs a descriptor
                     * can exit to. So we create fake bb objects for each
                     * block, purely to track this.
                     */

                    if (data_bb)
                        APPENDU(data_bb->outblocks, bb_get(label));

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
            switch (insn.em_arg.ema_argtype)
            {
                case cst_ptyp:
                    data_bss(EM_bsssize, insn.em_cst);
                    break;
                    
                default:
                    unknown_type("bss");
            }
            break;
        }

		case ps_pro: /* procedure start */
            current_proc = calloc(sizeof(struct procedure), 1);
            current_proc->name = strdup(insn.em_pnam);
            current_proc->root_bb = bb_get(current_proc->name);
            current_proc->nlocals = insn.em_nlocals;
            code_bb = current_proc->root_bb;
            code_bb->is_root = true;
            APPEND(current_proc->blocks, code_bb);
            break;

		case ps_end: /* procedure end */
            tb_procedure(current_proc);
            compile(current_proc);

            current_proc = NULL;
            code_bb = NULL;
			break;

		default:
            fatal("unknown pseudo with opcode %d\n", insn.em_opcode);
	}
}

static arith mes_get_cst(void)
{
    EM_getinstr(&insn);
    if (insn.em_type != EM_MESARG)
        fatal("malformed MES");
    return insn.em_cst;
}

static void parse_mes(void)
{
    assert(insn.em_arg.ema_argtype == cst_ptyp);
    switch (insn.em_cst)
    {
        case 0: /* error */
            fatal("MES 0 received (explicit halt)");

        case 3: /* register variable */
        {
            arith offset = mes_get_cst();
            int size = mes_get_cst();
            int type = mes_get_cst();
            int priority = mes_get_cst();
            tb_regvar(offset, size, type, priority);
            break;
        }
    }

    while ((insn.em_type == EM_STARTMES) || (insn.em_type == EM_MESARG))
        EM_getinstr(&insn);

    if (insn.em_type != EM_ENDMES)
        fatal("malformed MES");
}

void parse_em(void)
{
    EM_getinstr(&insn);
	tb_filestart();

	while (insn.em_type != EM_EOF)
	{
        switch (insn.em_type)
        {
            case EM_PSEU:
				parse_pseu();
                break;

            case EM_DEFILB:
                queue_ilabel(insn.em_ilb);
                break;

            case EM_DEFDLB:
            {
                const char* label = dlabel_to_str(insn.em_dlb);
                data_label(label);
                data_bb = bb_get(label);
                break;
            }

            case EM_DEFDNAM:
                data_label(strdup(insn.em_dnam));
                break;

            case EM_STARTMES:
                parse_mes();
                break;

            case EM_MNEM:
                if (code_bb)
                {
                    int flags = em_flag[insn.em_opcode - sp_fmnem];

                    if (flags & EM_PAR)
                    {
                        switch (insn.em_argtype)
                        {
                            case ilb_ptyp:
                                queue_insn_ilabel(insn.em_opcode, insn.em_ilb);
                                break;

                            case nof_ptyp:
                                queue_insn_label(insn.em_opcode,
                                    dlabel_to_str(insn.em_dlb), insn.em_off);
                                break;

                            case sof_ptyp:
                                queue_insn_label(insn.em_opcode,
                                    strdup(insn.em_dnam), insn.em_off);
                                break;

                            case pro_ptyp:
                                queue_insn_label(insn.em_opcode,
                                    strdup(insn.em_pnam), 0);
                                break;

                            case cst_ptyp:
                                if ((flags & EM_PAR) == PAR_B)
                                    queue_insn_ilabel(insn.em_opcode, insn.em_ilb);
                                else
                                    queue_insn_value(insn.em_opcode, insn.em_cst);
                                break;

                            default:
                                unknown_type("instruction");
                        }
                    }
                    else
                        queue_insn_simple(insn.em_opcode);
                }
                break;

            default:
                fatal("unrecognised instruction type '%d'", insn.em_type);
        }

		EM_getinstr(&insn);
	}

	tb_fileend();
}

/* vim: set sw=4 ts=4 expandtab : */
