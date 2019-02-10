/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "ack.h"
#include "list.h"
#include "trans.h"

/* Include once without redefining EXTERN, to declare all the symbols as extern. */
#include "data.h"

/* And again without EXTERN, to define them here. Without the extern versions above
 * then these symbols will all end up as COMMON, which is poorly supported on OSX. */
#undef EXTERN
#define EXTERN
#include "data.h"
