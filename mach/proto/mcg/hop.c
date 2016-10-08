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

void hop_print(char k, struct hop* hop)
{
	int i, j;
	bool soi = true;

	i = 0;
	for (i=0; i<hop->insels.count; i++)
	{
		struct insel* insel = hop->insels.item[i];

		if (soi)
		{
			tracef(k, "%c: %d from $%d:", k, hop->id, hop->ir->id);

			for (j=0; j<hop->ins.count; j++)
				tracef(k, " r%%%d", hop->ins.item[j]->id);
			for (j=0; j<hop->throughs.count; j++)
				tracef(k, " =%%%d", hop->throughs.item[j]->id);
			for (j=0; j<hop->outs.count; j++)
				tracef(k, " w%%%d", hop->outs.item[j]->id);
			tracef(k, " ");

			soi = false;
		}

		switch (insel->type)
		{
			case INSEL_EOI:
				tracef(k, "\n");
				soi = true;
				break;

			case INSEL_VREG:
				tracef(k, "%%%d", insel->u.vreg->id);
				break;

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

