/* Access to the prevalidated machine description during phase initialization.
 * Settings and strings are borrowed and must not outlive the callback.
 * mach_condtab() returns an optimizer-owned copy. */
#ifndef EGO_MACH_H
#define EGO_MACH_H

#include <libconfig.h>

config_setting_t* mach_setting(config_t* config, const char* path);
int mach_int(config_setting_t* setting, const char* name);
int mach_bool(config_setting_t* setting, const char* name);
const char* mach_string(config_setting_t* setting, const char* name);
cond_p mach_condtab(config_setting_t* setting);

#endif
