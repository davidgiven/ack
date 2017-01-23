#include "mcg.h"

static int hop_count = 1;
static struct hop* current_hop;
static char* buffer = NULL;
static int bufferlen = 0;
static int buffersize = 0;

static const struct burm_emitter_data emitter_data;

struct hop* new_hop(struct basicblock* bb, struct ir* ir)
{
	struct hop* hop = heap_alloc(&proc_heap, 1, sizeof(*hop));
	hop->id = hop_count++;
    hop->bb = bb;
	hop->ir = ir;
	return hop;
}

struct hop* new_move_hop(struct basicblock* bb)
{
	struct hop* hop = heap_alloc(&proc_heap, 1, sizeof(*hop));
	hop->id = hop_count++;
    hop->bb = bb;
    hop->is_move = true;
	return hop;
}

struct valueusage* hop_get_value_usage(struct hop* hop, struct value* value)
{
    struct valueusage* usage;

    if (!hop->valueusage)
    {
        hop->valueusage = heap_alloc(&proc_heap, 1, sizeof(struct hashtable));
        *hop->valueusage = empty_hashtable_of_values;
    }

    usage = hashtable_get(hop->valueusage, value);
    if (!usage)
    {
        usage = heap_alloc(&proc_heap, 1, sizeof(struct hashtable));
        hashtable_put(hop->valueusage, value, usage);
    }

    return usage;
}

void hop_add_move(struct hop* hop, struct value* src, struct value* dest)
{
    struct valueusage* usage;

    assert(hop->insels.count == 0);
    hop_get_value_usage(hop, src)->input = true;
    hop_get_value_usage(hop, dest)->output = true;
    hop->is_move = true;
}

static struct insel* new_insel(enum insel_type type)
{
	struct insel* insel = heap_alloc(&proc_heap, 1, sizeof(*insel));
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

void hop_add_vreg_insel(struct hop* hop, struct value* value, int index)
{
	struct insel* insel = new_insel(INSEL_VREG);
	insel->u.value = value;
    insel->index = index;
	array_append(&hop->insels, insel);
}

void hop_add_value_insel(struct hop* hop, struct ir* ir)
{
	struct insel* insel = new_insel(INSEL_VALUE);
	insel->u.ir = ir;
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
                    hop_add_vreg_insel(hop, va_arg(ap, struct value*), index);
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

void hop_walk(hop_walker_t* callback, void* user)
{
    int i, j, k;

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];

        for (j=0; j<bb->hops.count; j++)
        {
            struct hop* hop = bb->hops.item[j];
            callback(hop, user);
        }
    }
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

static struct vreg* actual(struct vreg* vreg)
{
    while (vreg->coalesced_with)
        vreg = vreg->coalesced_with;
    return vreg;
}

void appendvreg(struct vreg* vreg)
{
    appendf("%%%d", vreg->id);
    if (vreg->hreg != -1)
        appendf("=%d", vreg->hreg);
    if (vreg->is_spilt)
        appendf("S");
}

void appendvalue(struct hop* hop, struct value* value)
{
    struct valueusage* usage = hop_get_value_usage(hop, value);
    struct vreg* vreg = NULL;

    if (usage->input && usage->output && usage->invreg && usage->outvreg)
    {
        appendf("%%%d->%%%d", usage->invreg->id, usage->outvreg->id);
        return;
    }

    if (usage->input)
        vreg = usage->invreg;
    if (usage->output)
        vreg = usage->outvreg;
    if (usage->through)
    {
        assert(usage->invreg == usage->outvreg);
        vreg = usage->invreg;
    }

    if (vreg)
        appendvreg(actual(vreg));
    else
        appendf("$%d:%d", value->ir->id, value->subid);
}

static void appendheader(struct hop* hop)
{
    int i;

    appendf("$%d.%d", hop->value->ir->id, hop->value->subid);
    if (hop->ir)
        appendf(" from $%d", hop->ir->id);
    appendf(":");

    {
        struct hashtable_iterator hit = {};
        while (hashtable_next(hop->valueusage, &hit))
        {
            struct value* value = hit.key;
            struct valueusage* usage = hit.value;

            if (usage->input || usage->output || usage->through || usage->corrupted)
            {
                appendf(" ");
                if (usage->input)
                    appendf("r");
                if (usage->output)
                    appendf("w");
                if (usage->through)
                    appendf("=");
                if (usage->corrupted)
                    appendf("!");
                appendvalue(hop, value);
            }
        }
    }

    appendf(" ");
}

char* hop_render(struct hop* hop)
{
    int i;

    appendf(""); /* ensure the buffer has been allocated */
    bufferlen = 0;
	buffer[0] = '\0';

    if (hop->is_move)
    {
        struct hashtable_iterator hit = {};
        appendf("@move:");
        while (hashtable_next(hop->valueusage, &hit))
        {
            struct valueusage* usage = hit.value;
            appendf(" ");
            if (usage->input && usage->invreg)
                appendvreg(actual(usage->invreg));
            appendf("->");
            if (usage->output && usage->outvreg)
                appendvreg(actual(usage->outvreg));
        }
        appendf("\n");
        return buffer;
    }

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
                appendvalue(hop, insel->u.value);
                if (insel->index)
                    appendf(".%d", insel->index);
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
				struct ir* ir = insel->u.ir;
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
    char* header;

    if (!tracing(k))
        return;

    appendf(""); /* ensure the buffer has been allocated */
    bufferlen = 0;
	buffer[0] = '\0';

    appendheader(hop);
    header = strdup(buffer);

    hop_render(hop);

    p = strtok(buffer, "\n");
    tracef(k, "%c: %s", k, header);
    while (p)
    {
        tracef(k, "%s", p);
        p = strtok(NULL, "\n");
        if (p)
        {
            tracef(k, "\n");
            tracef(k, "%c: %s", k, header);
        }
    }
    tracef(k, "\n");
}

/* vim: set sw=4 ts=4 expandtab : */

