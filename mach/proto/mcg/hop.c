#include "mcg.h"

static int hop_count = 1;
static struct hop* current_hop;
static char* buffer = NULL;
static int bufferlen = 0;
static int buffersize = 0;

static const struct burm_emitter_data emitter_data;

struct hop* new_hop(struct basicblock* bb, struct ir* ir)
{
	struct hop* hop = calloc(1, sizeof *hop);
	hop->id = hop_count++;
    hop->bb = bb;
	hop->ir = ir;
	return hop;
}

static struct insel* new_insel(enum insel_type type)
{
	struct insel* insel = calloc(1, sizeof(*insel));
	insel->type = type;
	return insel;
}

void hop_add_string_insel(struct hop* hop, const char* string)
{
	struct insel* insel = new_insel(INSEL_STRING);
	insel->u.string = string;
	array_append(&hop->insels, insel);
}

void hop_add_hreg_insel(struct hop* hop, struct hreg* hreg, int index)
{
	struct insel* insel = new_insel(INSEL_HREG);
	insel->u.hreg = hreg;
    insel->index = index;
	array_append(&hop->insels, insel);
}

void hop_add_vreg_insel(struct hop* hop, struct vreg* vreg, int index)
{
	struct insel* insel = new_insel(INSEL_VREG);
	insel->u.vreg = vreg;
    insel->index = index;
	array_append(&hop->insels, insel);
}

void hop_add_value_insel(struct hop* hop, struct ir* ir)
{
	struct insel* insel = new_insel(INSEL_VALUE);
	insel->u.value = ir;
	array_append(&hop->insels, insel);
}

void hop_add_st_offset_insel(struct hop* hop, struct hreg* hreg)
{
	struct insel* insel = new_insel(INSEL_ST_OFFSET);
	insel->u.hreg = hreg;
	array_append(&hop->insels, insel);
}

void hop_add_ab_offset_insel(struct hop* hop, int offset)
{
	struct insel* insel = new_insel(INSEL_AB_OFFSET);
	insel->u.offset = offset;
	array_append(&hop->insels, insel);
}

void hop_add_lb_offset_insel(struct hop* hop, int offset)
{
	struct insel* insel = new_insel(INSEL_LB_OFFSET);
	insel->u.offset = offset;
	array_append(&hop->insels, insel);
}

void hop_add_eoi_insel(struct hop* hop)
{
	struct insel* insel = new_insel(INSEL_EOI);
	array_append(&hop->insels, insel);
}

void hop_add_insel(struct hop* hop, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            int index = 0;
            fmt += 2;
        again:
            switch (fmt[-1])
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    index = fmt[-1] - '0';
                    fmt++;
                    goto again;

                case 'd':
                    hop_add_string_insel(hop, aprintf("%d", va_arg(ap, int)));
                    break;

                case 's':
                    hop_add_string_insel(hop, va_arg(ap, const char*));
                    break;

                case 'S':
                    hop_add_st_offset_insel(hop, va_arg(ap, struct hreg*));
                    break;

                case 'A':
                    hop_add_ab_offset_insel(hop, va_arg(ap, int));
                    break;

                case 'L':
                    hop_add_lb_offset_insel(hop, va_arg(ap, int));
                    break;

                case 'H':
                    hop_add_hreg_insel(hop, va_arg(ap, struct hreg*), index);
                    break;

                case 'V':
                    hop_add_vreg_insel(hop, va_arg(ap, struct vreg*), index);
                    break;
            }
        }
        else
        {
            const char* end = strchr(fmt, '%');
            const char* s;
            if (end)
            {
                int len = end - fmt;
                s = strndup(fmt, len);
                fmt = end;
            }
            else
            {
                s = fmt;
                fmt += strlen(fmt);
            }

            hop_add_string_insel(hop, s);
        }
    }

    hop_add_eoi_insel(hop);
    va_end(ap);
}

static void print_header(char k, struct hop* hop)
{
    int i;

    tracef(k, "%c: %d", k, hop->id);
    if (hop->ir)
        tracef(k, " from $%d", hop->ir->id);
    tracef(k, ":");

    for (i=0; i<hop->ins.count; i++)
        tracef(k, " r%%%d", hop->ins.item[i]->id);
    for (i=0; i<hop->throughs.count; i++)
        tracef(k, " =%%%d", hop->throughs.item[i]->id);
    for (i=0; i<hop->outs.count; i++)
        tracef(k, " w%%%d", hop->outs.item[i]->id);
    tracef(k, " ");
}

static char* appendf(const char* fmt, ...)
{
    int n;
    char* p;
    va_list ap;

    va_start(ap, fmt);
    n = bufferlen + vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    if (n > buffersize)
    {
        buffersize *= 2;
		if (buffersize < n)
			buffersize = n*2;
        buffer = realloc(buffer, buffersize);
    }

    va_start(ap, fmt);
    vsprintf(buffer+bufferlen, fmt, ap);
    va_end(ap);

    bufferlen = n - 1; /* remember the \0 at the end */
    return p;
}

char* hop_render(struct hop* hop)
{
    int i;

    appendf(""); /* ensure the buffer has been allocated */
    bufferlen = 0;
	buffer[0] = '\0';

	for (i=0; i<hop->insels.count; i++)
	{
		struct insel* insel = hop->insels.item[i];

		switch (insel->type)
		{
			case INSEL_EOI:
				appendf("\n");
				break;

            case INSEL_HREG:
            {
                struct hreg* hreg = insel->u.hreg;
                if (hreg->brd)
                    appendf("%s", hreg->brd->names[insel->index]);
                else
                    appendf("%s.%d", hreg->id, insel->index);
                break;
            }

			case INSEL_VREG:
            {
                struct vreg* vreg = insel->u.vreg;
                struct hreg* hreg = pmap_findright(&hop->regsin, vreg);
                if (!hreg)
                    hreg = pmap_findright(&hop->regsout, vreg);
                if (hreg)
                    appendf("%s", hreg->brd->names[insel->index]);
                else
                    appendf("%%%d.%d", vreg->id, insel->index);
				break;
            }

			case INSEL_STRING:
				appendf("%s", insel->u.string);
				break;

            case INSEL_ST_OFFSET:
                appendf("%d", current_proc->fp_to_sb + insel->u.hreg->offset);
                break;

            case INSEL_AB_OFFSET:
                appendf("%d", current_proc->fp_to_ab + insel->u.offset);
                break;

            case INSEL_LB_OFFSET:
                appendf("%d", current_proc->fp_to_lb + insel->u.offset);
                break;

			case INSEL_VALUE:
			{
				struct ir* ir = insel->u.value;
				switch (ir->opcode)
				{
					case IR_BLOCK:
						appendf("%s", platform_label(ir->u.bvalue->name));
						break;

					case IR_LABEL:
						appendf("%s", platform_label(ir->u.lvalue));
						break;

					case IR_LOCAL:
                        if (ir->u.ivalue >= 0)
                            appendf("%d", current_proc->fp_to_ab + ir->u.ivalue);
                        else
                            appendf("%d", current_proc->fp_to_lb + ir->u.ivalue);
                        break;

					case IR_CONST:
						appendf("%d", ir->u.ivalue);
						break;

                    default:
                        assert(false);
				}
				break;
			}

            default:
                assert(false);
		}
	}

    return buffer;
}

void hop_print(char k, struct hop* hop)
{
	int i;
	bool soi = false;
    char* p;

    hop_render(hop);

    p = strtok(buffer, "\n");
    print_header(k, hop);
    while (p)
    {
        tracef(k, "%s", p);
        p = strtok(NULL, "\n");
        if (p)
        {
            tracef(k, "\n");
            print_header(k, hop);
        }
    }
    tracef(k, "\n");
}

/* vim: set sw=4 ts=4 expandtab : */

