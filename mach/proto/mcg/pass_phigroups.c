#include "mcg.h"

static PMAPOF(struct vreg, struct vreg) phimap;

static void make_phimap(void)
{
	int i, j;

	phimap.count = 0;
	for (i=0; i<cfg.preorder.count; i++)
	{
		struct basicblock* bb = cfg.preorder.item[i];
		
		for (j=0; j<bb->phis.count; j++)
		{
			struct vreg* vreg = bb->phis.item[j].left;
			struct phi* phi = bb->phis.item[j].right;
			struct vreg* prevvreg = phi->ir->result;

			pmap_add(&phimap, vreg, prevvreg);
		}
	}
}

static void recursively_associate_group(struct phicongruence* c, struct vreg* vreg)
{
	int i;

	vreg->congruence = c;
	array_appendu(&c->vregs, vreg);
    tracef('V', "V: %%%d is a member of congruence group %d\n",
        vreg->id, c->id);

    if (vreg->defined)
    {
        struct constraint* constraint = pmap_findleft(&vreg->defined->constraints, vreg);
        if (c->type == 0)
            c->type = vreg->type;

        if (c->type != vreg->type)
            fatal("tried to add register %%%d of type 0x%x to a phi congruence group of type 0x%x",
                vreg->id, vreg->type, c->type);

        array_appendu(&c->definitions, vreg->defined);
    }

	for (;;)
	{
		struct vreg* child = pmap_findleft(&phimap, vreg);
        if (!child)
            break;

        pmap_remove(&phimap, vreg, child);
        recursively_associate_group(c, child);
    }
		
    for (;;)
    {
		struct vreg* child = pmap_findright(&phimap, vreg);
		if (!child)
            break;

        pmap_remove(&phimap, child, vreg);
        recursively_associate_group(c, child);
	}
}

static void update_vreg_types(struct phicongruence* c)
{
    int i;

    for (i=0; i<c->vregs.count; i++)
    {
        struct vreg* vreg = c->vregs.item[i];

        if (vreg->type == 0)
            vreg->type = c->type;
        assert(vreg->type == c->type);
        assert(vreg->type != 0);
    }
}

static void associate_groups(void)
{
    static int number = 0;

	while (phimap.count > 0)
	{
		struct phicongruence* c = calloc(1, sizeof(*c));
        c->id = number++;
		recursively_associate_group(c, phimap.item[0].left);
        update_vreg_types(c);
	}
}

void pass_find_phi_congruence_groups(void)
{
	make_phimap();
	associate_groups();
}

/* vim: set sw=4 ts=4 expandtab : */



