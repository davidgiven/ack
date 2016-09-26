#include "mcg.h"

static char outbuf[256];
static struct hop* current_hop;

static const struct burm_emitter_data emitter_data;

struct hop* new_hop(int insn_no, struct ir* ir)
{
	struct hop* hop = calloc(1, sizeof *hop);
	hop->insn_no = insn_no;
	hop->ir = ir;
	return hop;
}

static void append(const char* fmt, ...)
{
	va_list ap;
	
	va_start(ap, fmt);
	vsprintf(outbuf + strlen(outbuf), fmt, ap);
	va_end(ap);
}

static void emit_reg(struct ir* ir)
{
	struct hop* hop = ir->hop;
	if (hop)
	{
		/* Reference to another hop must be its result register. */
		append("%%%d", hop->resultvreg->id);
	}
	else
	{
		/* ...if there is no hop, it's a fragment. */
		const struct burm_instruction_data* insndata = &burm_instruction_data[ir->insn_no];
        insndata->emitter(ir, &emitter_data);
	}
}

static void emit_string(const char* data)
{
	append("%s", data);
}

static void emit_value(struct ir* ir)
{
	append("(val)");
}

static void emit_resultreg(void)
{
	append("%%%d", current_hop->resultvreg->id);
}

static void emit_eoi(void)
{
	append("\n");
}

static const struct burm_emitter_data emitter_data =
{
    &emit_string,
    &emit_reg,
    &emit_value,
    &emit_resultreg,
    &emit_eoi
};

void hop_print(char k, struct hop* hop)
{
	char* s;
    const struct burm_instruction_data* insndata = &burm_instruction_data[hop->insn_no];

	current_hop = hop;
	outbuf[0] = 0;
	if (insndata->emitter)
	{
		insndata->emitter(hop->ir, &emitter_data);

		s = strtok(outbuf, "\n");
		do
		{
			tracef(k, "%c: %p: %s\n", k, hop, s);
			s = strtok(NULL, "\n");
		}
		while (s);
	}
	else
		tracef(k, "%c: %p: (empty)\n", k, hop);
}

