/*
    <float.h> -- simple version used by "gimplify"

    last edit:  2007-02-12  D A Gwyn
*/

/* Does not exactly fit any model, and is minimal for "universality". */

#define FLT_ROUNDS  (-1)
#define FLT_EVAL_METHOD (-1)
#define FLT_RADIX   2
#define DECIMAL_DIG 10

/* assumes that "gimplify" specifies "-Dfloat=double" */
#define FLT_MANT_DIG    10
#define FLT_EPSILON (1E-9F)
#define FLT_DIG     10
#define FLT_MIN_EXP (-31)
#define FLT_MIN     (1E-37F)
#define FLT_MIN_10_EXP  (-37)
#define FLT_MAX_EXP 37
#define FLT_MAX     (1E+37F)
#define FLT_MAX_10_EXP  37

#define DBL_MANT_DIG    10
#define DBL_EPSILON (1E-9)
#define DBL_DIG     10
#define DBL_MIN_EXP (-31)
#define DBL_MIN     (1E-37)
#define DBL_MIN_10_EXP  (-37)
#define DBL_MAX_EXP 37
#define DBL_MAX     (1E+37)
#define DBL_MAX_10_EXP  37

#define LDBL_MANT_DIG   10
#define LDBL_EPSILON    (1E-9L)
#define LDBL_DIG    10
#define LDBL_MIN_EXP    (-31)
#define LDBL_MIN    (1E-37L)
#define LDBL_MIN_10_EXP (-37)
#define LDBL_MAX_EXP    37
#define LDBL_MAX    (1E+37L)
#define LDBL_MAX_10_EXP 37
