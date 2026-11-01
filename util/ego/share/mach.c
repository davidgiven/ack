/* Read named values and copy cost tables from a libconfig description. */
#include <string.h>
#include "types.h"
#include "debug.h"
#include "alloc.h"
#include "mach.h"

config_setting_t* mach_setting(config_t* config, const char* path)
{
	config_setting_t* setting = config_lookup(config, path);
	if (!setting)
		error("missing machine setting %s", path);
	return setting;
}

int mach_int(config_setting_t* setting, const char* name)
{
	int value;
	if (!config_setting_lookup_int(setting, name, &value))
		error("missing or invalid machine integer %s", name);
	return value;
}

int mach_bool(config_setting_t* setting, const char* name)
{
	int value;
	if (!config_setting_lookup_bool(setting, name, &value))
		error("missing or invalid machine boolean %s", name);
	return value;
}

const char* mach_string(config_setting_t* setting, const char* name)
{
	const char* value;
	if (!config_setting_lookup_string(setting, name, &value))
		error("missing or invalid machine string %s", name);
	return value;
}

cond_p mach_condtab(config_setting_t* setting)
{
	int i, length = config_setting_length(setting);
	cond_p table = newcondtab(length);
	for (i = 0; i < length; i++)
	{
		config_setting_t* rule = config_setting_get_elem(setting, i);
		const char* when = mach_string(rule, "when");
		if (!strcmp(when, "default"))
			table[i].mc_cond = DEFAULT;
		else if (!strcmp(when, "fitbyte"))
			table[i].mc_cond = FITBYTE;
		else if (!strcmp(when, "in_0_63"))
			table[i].mc_cond = IN_0_63;
		else if (!strcmp(when, "in_0_8"))
			table[i].mc_cond = IN_0_8;
		else
			error("unknown machine condition %s", when);
		table[i].mc_tval = mach_int(rule, "time");
		table[i].mc_sval = mach_int(rule, "space");
	}
	return table;
}
