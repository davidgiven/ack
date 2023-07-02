#include "mcg.h"

static int hop_count = 0;
static struct hop* current_hop;
static struct buffer renderbuf;
static struct mempool hoppool;

static const struct burm_emitter_data emitter_data;

void clear_hops(void)
{
	tracef('M', "M: hop mempool was %d bytes\n", hoppool.size);
	mempool_reset(&hoppool);
	hop_count = 0;
}

struct constraint* get_constraint(struct hop* hop, struct vreg* vreg)
{
	struct constraint* c = pmap_findleft(&hop->constraints, vreg);
	if (!c)
	{
		c = mempool_alloc(&hoppool, sizeof(*c));
		pmap_put(&hop->constraints, vreg, c);
	}
	return c;
}

struct hop* new_hop(struct basicblock* bb, struct ir* ir)
{
	struct hop* hop = mempool_alloc(&hoppool, sizeof *hop);
	hop->id = ++hop_count;
	hop->bb = bb;
	hop->ir = ir;
	return hop;
}

static struct insel* new_insel(enum insel_type type)
{
	struct insel* insel = mempool_alloc(&hoppool, sizeof(*insel));
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
	struct insel* insel = new_insel(INSEL_SB_OFFSET);
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
				case '%':
					hop_add_string_insel(hop, "%");
					break;

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

	for (i = 0; i < hop->ins.count; i++)
		tracef(k, " r%%%d", hop->ins.item[i]->id);
	for (i = 0; i < hop->throughs.count; i++)
		tracef(k, " =%%%d", hop->throughs.item[i]->id);
	for (i = 0; i < hop->outs.count; i++)
		tracef(k, " w%%%d", hop->outs.item[i]->id);
	tracef(k, " ");
	if (hop->is_copy)
		tracef(k, "(COPY) ");
}

static void appendf(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	buffer_appendfv(&renderbuf, fmt, ap);
	va_end(ap);
}

char* hop_render(struct hop* hop)
{
	int i;

	buffer_clear(&renderbuf);

	for (i = 0; i < hop->insels.count; i++)
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
				if (vreg->coalesced_with)
					vreg = vreg->coalesced_with;
				if (vreg->hreg)
					appendf("%s", vreg->hreg->brd->names[insel->index]);
				else
				{
					while (vreg)
					{
						appendf("%%%d", vreg->id);
						vreg = vreg->next_coalesced_register;
						if (vreg)
							appendf("+");
					}
				}
				break;
			}

			case INSEL_STRING:
				appendf("%s", insel->u.string);
				break;

			case INSEL_SB_OFFSET:
				appendf("%d", current_proc->fp_to_sb + insel->u.offset);
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
							appendf(
							    "%d", current_proc->fp_to_ab + ir->u.ivalue);
						else
							appendf(
							    "%d", current_proc->fp_to_lb + ir->u.ivalue);
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

	return renderbuf.ptr;
}

void hop_print(char k, struct hop* hop)
{
	int i;
	bool soi = false;
	char* p;

	print_header(k, hop);
	for (i = 0; i < hop->constraints.count; i++)
	{
		struct vreg* vreg = hop->constraints.item[i].left;
		struct constraint* constraint = hop->constraints.item[i].right;
		tracef(
		    k, " %s/%s:", render_vreg(vreg), render_regclass(vreg->regclass));
		tracef(k, "%s", render_regclass(constraint->regclass));
		if (constraint->equals_to)
			tracef(k, ":=%s", render_vreg(constraint->equals_to));
		if (constraint->preserved)
			tracef(k, ":preserved");
	}
	tracef(k, "\n");

	hop_render(hop);

	p = strtok(renderbuf.ptr, "\n");
	print_header(k, hop);
	tracef(k, "insels: ");
	while (p)
	{
		tracef(k, "%s", p);
		p = strtok(NULL, "\n");
		if (p)
		{
			tracef(k, "\n");
			print_header(k, hop);
			tracef(k, "insels: ");
		}
	}
	tracef(k, "\n");
}

/* vim: set sw=4 ts=4 expandtab : */
