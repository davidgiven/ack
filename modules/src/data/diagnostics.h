#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

extern const char* program_name;

extern void warning(const char* fmt, ...);
extern void fatal(const char* fmt, ...);

#endif

/* vim: set sw=4 ts=4 expandtab : */

