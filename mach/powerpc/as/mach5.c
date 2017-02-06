/*
 * To relocate hi16[expr] or ha16[expr], we must find a matching
 * lo16[expr] in a later instruction.  This is because RELOPPC needs a
 * hi16/lo16 or ha16/lo16 pair.  Also, RELOPPC only works with certain
 * specific opcodes.
 */

static item_t *last_und_item;

/* Info about the current hi16, ha16, or lo16 */
static int hl_token;
static expr_t hl_expr;
static item_t* hl_item;             /* undefined symbol or NULL */

/* Info about each hi16 or ha16 relocation */
struct hirel {
    expr_t          hr_expr;
    struct hirel*   hr_next;        /* next hirel in source order */
    struct hirel*   hr_inext;       /* next incomplete hirel */
    item_t*         hr_item;        /* undefined symbol or NULL */
    long            hr_lineno;
    ADDR_T          hr_hidot;       /* address of hi16/ha16 */
    ADDR_T          hr_lodot;       /* address of matching lo16 */
    short           hr_dottyp;      /* section */
    short           hr_token;       /* OP_HI or OP_HA */
};
static struct hirel* hr_head = NULL;
static struct hirel* hr_tail = NULL;
static struct hirel* hr_ihead = NULL;

/* Bitset to remember if each lo16 relocation is lonely */
static uint16_t* lo_set = NULL;
static int lo_count2 = 0;           /* pass 2 */
static int lo_count3 = 0;           /* pass 3 */


static int is_und(int typ)
{
    return (typ & S_TYP) == S_UND || typ & S_COM;
}

void machload(item_t* item)
{
    if (is_und(item->i_type)) {
        last_und_item = item;
    }
}

void no_hl(void) {
    hl_token = 0;
}

word_t eval_hl(expr_t* expr, int token)
{
    struct hirel* hr;
    ADDR_T distance;
    word_t val = expr->val;
    uint16_t hi = val >> 16;
    uint16_t lo = val & 0xffff;

    hl_token = token;
    hl_expr = *expr;
    if (is_und(expr->typ))
        hl_item = last_und_item;
    else
        hl_item = NULL;

    if (pass == PASS_3) {
        hr = hr_head;
        if (hr && hr->hr_hidot == DOTVAL && hr->hr_dottyp == DOTTYP) {
            /*
             * We have a hirel from emit_hl() PASS_2.  Encode distance
             * to the matching lo16 in top 6 bits of hi.
             */
            distance = hr->hr_lodot - DOTVAL;
            if (distance & 0x0003)
                serror("matching lo16 is misaligned");
            if (distance & ~0x00fc)
                serror("matching lo16 is too far away");
            fit(fitx((int16_t)hi, 10));
            hi = (distance << 8) | (hi & 0x03ff);

            /* Remove hirel from list. */
            hr_head = hr->hr_next;
            free(hr);

            /* No sign adjustment; linker will do it later. */
            return hi;
        }
    }

    switch (token) {
    case OP_HA:  /* ha16[expr] */
        /*
         * If the low half will be treated as a signed value, then
         * values greater than 0x7fff will cause the high half to have
         * 1 subtracted from it; so we apply an adjustment here.
         */
        if (lo > 0x7fff)
            hi++;
        /* FALLTHROUGH */
    case OP_HI:  /* hi16[expr] */
        return hi;
    case OP_LO:  /* lo16[expr] */
        return lo;
    }
}

static void cant_use_hl(const char *what)
{
    serror("can't use %s in this instruction", what);
}

static int hr_match(const struct hirel* hr, int und)
{
    /*
     * Compares hr (hi16 or ha16) with hl_expr (lo16).  They match,
     * completing a pair, if they share the same expression and
     * symbol, and they are in the same section.
     */
    return (hr->hr_expr.typ == hl_expr.typ &&
            hr->hr_expr.val == hl_expr.val &&
            (!und || hr->hr_item == hl_item) &&
            hr->hr_dottyp == DOTTYP);
}

static void need_hi(const struct hirel* hr, const char* needed,
                    const char* got)
{
    long old = lineno;
    lineno = hr->hr_lineno;
    serror("need %s, not %s, here", needed, got);
    lineno = old;
}

void emit_hl(word_t in)
{
    struct hirel *hr, **hrlink;
    struct lorel *lr;
    word_t opcode;
    int needrelo, und;
    uint16_t bit, *rset;

    switch (hl_token) {
    case OP_HI:  /* hi16[...] */
    case OP_HA:  /* ha16[...] */
        if ((hl_expr.typ & S_TYP) != S_ABS && PASS_RELO) {
            if (pass == PASS_2) {
                opcode = in >> 26;
                if (opcode != 15 /* addis */)
                    cant_use_hl(hl_token == OP_HI ? "hi16" : "ha16");

                /* Make a new incomplete hirel. */
                hr = malloc(sizeof(*hr));
                if (!hr)
                    fatal("out of memory");
                hr->hr_expr = hl_expr;
                hr->hr_next = NULL;
                hr->hr_inext = hr_ihead;
                hr->hr_item = hl_item;
                hr->hr_lineno = lineno;
                hr->hr_hidot = DOTVAL;
                hr->hr_lodot = 0;
                hr->hr_dottyp = DOTTYP;
                hr->hr_token = hl_token;

                /* Add it to end of main list. */
                if (hr_tail) {
                    hr_tail->hr_next = hr;
                    hr_tail = hr;
                } else
                    hr_head = hr_tail = hr;
                /* Add it to list of incompletes. */
                hr_ihead = hr;
            }

            newrelo(hl_expr.typ, RELOPPC | FIXUPFLAGS);
        }
        break;
    case OP_LO:  /* lo16[...] */
        if ((hl_expr.typ & S_TYP) != S_ABS && PASS_RELO) {
            switch (pass) {
            case PASS_2:
                /* Search for hi16 or ha16 to complete. */
                und = is_und(hl_expr.typ);
                hrlink = &hr_ihead;
                while (hr = *hrlink) {
                    if (hr_match(hr, und)) {
                        /* Found hr, remove it from list. */
                        *hrlink = hr->hr_inext;
                        hr->hr_inext = NULL;
                        break;
                    }
                    hrlink = &(hr->hr_inext);
                }

                if (hr) {
                    hr->hr_lodot = DOTVAL;

                    opcode = in >> 26;
                    switch (opcode) {
                    case 14: /* addi */
                    case 34: /* lbz */
                    case 48: /* lfs */
                    case 50: /* lfd */
                    case 42: /* lha */
                    case 40: /* lhz */
                    case 32: /* lwz */
                    case 38: /* stb */
                    case 52: /* stfs */
                    case 54: /* stfd */
                    case 44: /* sth */
                    case 36: /* stw */
                        if (hr->hr_token == OP_HI)
                            need_hi(hr, "ha16", "hi16");
                        break;
                    case 24: /* ori */
                        if (hr->hr_token == OP_HA)
                            need_hi(hr, "hi16", "ha16");
                        break;
                    default:
                        cant_use_hl("lo16");
                    }
                }

                if ((lo_count2 % 128) == 0) {
                    /* Grow the bitset by 128 bits. */
                    rset = realloc(lo_set, (lo_count2 + 128) / 8);
                    if (!rset)
                        fatal("out of memory");
                    lo_set = rset;
                }

                bit = (1 << (lo_count2 & 0xf));
                if (hr) {
                    /* Completing RELOPPC, don't need RELO2. */
                    needrelo = 0;
                    lo_set[lo_count2 >> 4] &= ~bit;
                } else {
                    /* Lonely lo16, need RELO2. */
                    needrelo = 1;
                    lo_set[lo_count2 >> 4] |= bit;
                }
                lo_count2++;
                break;

            case PASS_3:
                /* Remember from PASS_2 if we need RELO2. */
                bit = (1 << (lo_count3 & 0xf));
                needrelo = (lo_set[lo_count3 >> 4] & bit);
                lo_count3++;
                break;
            }

            if (needrelo) {
                /* This is a lonely lo16 without hi16 or ha16. */
                DOTVAL += 2;
                newrelo(hl_expr.typ, RELO2 | FIXUPFLAGS);
                DOTVAL -= 2;
            } else {
                /*
                 * This lo16 completes a RELOPPC (generated by hi16 or
                 * ha16).  If we have a symbol for relocation, we need
                 * to abandon it.
                 */
                relonami = 0;
            }
        }
        break;
    }

    emit4(in);
}

void machfinish(int n) {
    long old;

    /* At end of pass 2, can't have any incomplete hirel. */
    if (n == PASS_2 && hr_ihead) {
        old = lineno;
        lineno = hr_ihead->hr_lineno;
        serror("%s without matching lo16",
               hr_ihead->hr_token == OP_HI ? "hi16" : "ha16");
        lineno = old;
    }
}
