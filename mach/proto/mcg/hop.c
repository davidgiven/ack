#include "mcg.h"

static int hop_count = 1;
static struct hop* current_hop;

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

void hop_add_hreg_insel(struct hop* hop, struct hreg* hreg)
{
	struct insel* insel = new_insel(INSEL_HREG);
	insel->u.hreg = hreg;
	array_append(&hop->insels, insel);
}

void hop_add_vreg_insel(struct hop* hop, struct vreg* vreg)
{
	struct insel* insel = new_insel(INSEL_VREG);
	insel->u.vreg = vreg;
	array_append(&hop->insels, insel);
}

void hop_add_value_insel(struct hop* hop, struct ir* ir)
{
	struct insel* insel = new_insel(INSEL_VALUE);
	insel->u.value = ir;
	array_append(&hop->insels, insel);
}

void hop_add_eoi_insel(struct hop* hop)
{
	struct insel* insel = new_insel(INSEL_EOI);
	array_append(&hop->insels, insel);
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

void hop_print(char k, struct hop* hop)
{
	int i;
	bool soi = false;

    print_header(k, hop);

	i = 0;
	for (i=0; i<hop->insels.count; i++)
	{
		struct insel* insel = hop->insels.item[i];

		if (soi)
		{
            print_header(k, hop);
			soi = false;
		}

		switch (insel->type)
		{
			case INSEL_EOI:
				tracef(k, "\n");
				soi = true;
				break;

            case INSEL_HREG:
            {
                struct hreg* hreg = insel->u.hreg;
                tracef(k, "%s", hreg->name);
                break;
            }

			case INSEL_VREG:
            {
                struct vreg* vreg = insel->u.vreg;
                struct hreg* hreg = pmap_findright(&hop->regsin, vreg);
                if (!hreg)
                    hreg = pmap_findright(&hop->regsout, vreg);
                if (hreg)
                    tracef(k, "%s", hreg->name);
                else
                    tracef(k, "%%%d", vreg->id);
				break;
            }

			case INSEL_STRING:
				tracef(k, "%s", insel->u.string);
				break;

			case INSEL_VALUE:
			{
				struct ir* ir = insel->u.value;
				switch (ir->opcode)
				{
					case IR_BLOCK:
						tracef(k, "%s", ir->u.bvalue->name);
						break;

					case IR_LABEL:
						tracef(k, "%s", ir->u.lvalue);
						break;

					case IR_LOCAL:
					case IR_CONST:
						tracef(k, "%d", ir->u.ivalue);
						break;
				}
				break;
			}
		}
	}

	if (!soi)
		tracef(k, "\n", k);
}

/* vim: set sw=4 ts=4 expandtab : */

