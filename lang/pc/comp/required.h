/* REQUIRED PROCEDURES AND FUNCTIONS */

enum
{
    R__UNUSED = 0,

    /* PROCEDURES */
    /* FILE HANDLING */
    R_REWRITE,
    R_PUT,
    R_RESET,
    R_GET,
    R_PAGE,

    /* DYNAMIC ALLOCATION */
    R_NEW,
    R_DISPOSE,

    /* MISCELLANEOUS PROCEDURE(S) */
    R_HALT,

    /* TRANSFER */
    R_PACK,
    R_UNPACK,

    /* FUNCTIONS */
    /* ARITHMETIC */
    R_ABS,
    R_SQR,
    R_SIN,
    R_COS,
    R_EXP,
    R_LN,
    R_SQRT,
    R_ARCTAN,

    /* TRANSFER */
    R_TRUNC,
    R_ROUND,

    /* ORDINAL */
    R_ORD,
    R_CHR,
    R_SUCC,
    R_PRED,

    /* BOOLEAN */
    R_ODD,
    R_EOF,
    R_EOLN,
};
