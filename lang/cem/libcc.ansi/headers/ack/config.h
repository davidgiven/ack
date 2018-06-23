#ifndef _ACK_CONFIG_H
#define _ACK_CONFIG_H

#include <ack/plat.h>

#ifndef ACKCONF_WANT_FLOAT
#define ACKCONF_WANT_FLOAT 1
#endif

#ifndef ACKCONF_WANT_STDIO_FLOAT
#define ACKCONF_WANT_STDIO_FLOAT ACKCONF_WANT_FLOAT
#endif

#ifndef ACKCONF_WANT_STANDARD_O
#define ACKCONF_WANT_STANDARD_O 1
#endif

#ifndef ACKCONF_WANT_STANDARD_SIGNALS
#define ACKCONF_WANT_STANDARD_SIGNALS 1
#endif

#ifndef ACKCONF_WANT_TERMIOS
/* Don't compile termios-using functions unless the plat explicitly asks for it. */
#define ACKCONF_WANT_TERMIOS 0
#endif

#endif
