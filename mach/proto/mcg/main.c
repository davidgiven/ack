#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "em.h"
#include "em_comp.h"
#include "em_pseu.h"
#include "em_mnem.h"
#include "em_flag.h"
#include "em_ptyp.h"

extern char em_pseu[][4];
extern char em_mnem[][4];
extern char em_flag[];

static void fatal(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");

	va_end(ap);
	exit(1);
}

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

int main(int argc, const char* argv[])
{
    struct e_instr insn;

	if (!EM_open(argv[1]))
		fatal("Couldn't open input file: %s", EM_error);
	
    EM_getinstr(&insn);
    printf("; word size = %d\n", EM_wordsize);
    printf("; pointer size = %d\n", EM_pointersize);

	while (insn.em_type != EM_EOF)
	{
        printf("%s %s ",
            type_to_str(insn.em_type),
            argtype_to_str(insn.em_arg.ema_argtype));

        switch (insn.em_type)
        {
            case EM_PSEU:
                printf("%s ", em_pseu[insn.em_opcode - sp_fpseu]);
                switch (insn.em_opcode)
                {
                    case ps_exp: /* external proc */
                    case ps_exa: /* external array */
                    case ps_inp: /* internal proc */
                    case ps_ina: /* internal array */
                        switch (insn.em_arg.ema_argtype)
                        {
                            case pro_ptyp:
                                printf("name=%s\n", insn.em_pnam);
                                break;

                            case sof_ptyp:
                                printf("name=%s offset=0x%x\n",
                                    insn.em_dnam,
                                    insn.em_off);
                                break;

                            default:
                                printf("name=?\n");
                        }
                        break;

                    case ps_con: /* .data */
                    case ps_rom: /* .rom */
                        printf("size=%d ",
                            insn.em_size);

                        switch (insn.em_arg.ema_argtype)
                        {
                            case ico_ptyp:
                            case uco_ptyp:
                            case fco_ptyp:
                            case str_ptyp:
                                printf("val=%s\n", insn.em_string);
                                break;

                            default:
                                printf("val=?\n");
                        }
                        break;

                    case ps_pro: /* procedure start */
                        printf("\n\n%s %d\n",
                            insn.em_arg.ema_pnam,
                            insn.em_arg.ema_szoroff);
                        break;

                    case ps_end: /* procedure end */
                        printf("%d\n\n\n",
                            insn.em_arg.ema_szoroff);
                        break;

                    default:
                        printf("???\n");
                }
                break;

            case EM_DEFILB:
                printf("code label %d\n", insn.em_ilb);
                break;

            case EM_DEFDLB:
                printf("data label %d\n", insn.em_dlb);
                break;

            case EM_DEFDNAM:
                printf("data label %s\n", insn.em_dnam);
                break;

            case EM_STARTMES:
                for (;;)
                {
                    switch (insn.em_arg.ema_argtype)
                    {
                        case cst_ptyp:
                            printf("%d ", insn.em_cst);
                            break;

                        case str_ptyp:
                            printf("%s ", insn.em_string);
                            break;

                        default:
                            printf("(unknown %s) ",
                                argtype_to_str(insn.em_arg.ema_argtype));
                    }

                    EM_getinstr(&insn);
                    if (insn.em_type == EM_ENDMES)
                        break;
                    assert(insn.em_type == EM_MESARG);
                 }
                 printf("\n");
                 break;

            case EM_MNEM:
            {
                int flag = em_flag[insn.em_opcode - sp_fmnem];
                printf("%s %c%c%c%c ",
                    em_mnem[insn.em_opcode - sp_fmnem],
                    "/CDNFLGWSZOPBR"[flag & EM_PAR],
                    (flag & FLO_C) ? 'c' : '.',
                    (flag & FLO_P) ? 'p' : '.',
                    (flag & FLO_T) ? 't' : '.');

                if (flag & EM_PAR)
                {
                    switch (insn.em_argtype)
                    {
                        case ilb_ptyp:
                            printf("ilb ");
                            break;

                        case nof_ptyp:
                            printf("nof ");
                            break;

                        case sof_ptyp:
                            printf("sof ");
                            break;

                        case cst_ptyp:
                            printf("cst 0x%08x ", insn.em_cst);
                            break;

                        case pro_ptyp:
                            printf("pro ");
                            break;

                        case str_ptyp:
                            printf("str ");
                            break;

                        case ico_ptyp:
                            printf("ico ");
                            break;

                        case uco_ptyp:
                            printf("uco ");
                            break;

                        case fco_ptyp:
                            printf("fco ");
                            break;

                        default:
                            printf("???");
                    }
                }
                printf("\n");
                break;
            }

            default:
                printf("%d\n", insn.em_opcode);
                break;
        }

		EM_getinstr(&insn);
	}

	EM_close();
	return 0;
}

/* vim: set sw=4 ts=4 expandtab : */
