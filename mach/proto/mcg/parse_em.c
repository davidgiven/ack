#include "mcg.h"

static struct e_instr insn;
static const char* current_proc;

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
    return aprintf("__%s_I%d", current_proc, l);
}

static const char* dlabel_to_str(label l)
{
    return aprintf("__D%d", l);
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
                    data_offset(ilabel_to_str(insn.em_ilb), 0, ro);
                    break;

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
            if (insn.em_nlocals == -1)
                fatal("procedures with unspecified number of locals are not supported yet");

            current_proc = strdup(insn.em_pnam);
            tb_procstart(current_proc, insn.em_nlocals);
            break;

		case ps_end: /* procedure end */
            tb_procend();
            current_proc = NULL;
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
                tb_ilabel(ilabel_to_str(insn.em_ilb));
                break;

            case EM_DEFDLB:
                data_label(dlabel_to_str(insn.em_dlb));
                break;

            case EM_DEFDNAM:
                data_label(strdup(insn.em_dnam));
                break;

            case EM_STARTMES:
                parse_mes();
                break;

            case EM_MNEM:
            {
                int flags = em_flag[insn.em_opcode - sp_fmnem];

                if (flags & EM_PAR)
                {
                    switch (insn.em_argtype)
                    {
                        case ilb_ptyp:
                            tb_insn_label(insn.em_opcode, flags,
                                ilabel_to_str(insn.em_ilb), 0);
                            break;

                        case nof_ptyp:
                            tb_insn_label(insn.em_opcode, flags,
                                dlabel_to_str(insn.em_dlb), insn.em_off);
                            break;

                        case sof_ptyp:
                            tb_insn_label(insn.em_opcode, flags,
                                strdup(insn.em_dnam), insn.em_off);
                            break;

                        case pro_ptyp:
                            tb_insn_label(insn.em_opcode, flags,
                                strdup(insn.em_pnam), 0);
                            break;

                        case cst_ptyp:
                            if ((flags & EM_PAR) == PAR_B)
                                tb_insn_label(insn.em_opcode, flags,
                                    ilabel_to_str(insn.em_ilb), 0);
                            else
                                tb_insn_value(insn.em_opcode, flags,
                                    insn.em_cst);
                            break;

                        default:
                            unknown_type("instruction");
                    }
                }
                else
                    tb_insn_simple(insn.em_opcode, flags);

                break;
            }

            default:
                fatal("unrecognised instruction type '%d'", insn.em_type);
        }

		EM_getinstr(&insn);
	}

	tb_fileend();
}

/* vim: set sw=4 ts=4 expandtab : */
