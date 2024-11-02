#ifndef DEFMODULE_H
#define DEFMODULE_H

extern char* getwdir(register char *fn);
extern struct def* GetDefinitionModule(register struct idf* id, int incr);

#endif
