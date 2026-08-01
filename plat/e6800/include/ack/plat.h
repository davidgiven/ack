/* $Source$
 * $State$
 * $Revision$
 */

#ifndef _ACK_PLAT_H
#define _ACK_PLAT_H

#define ACKCONF_WANT_STDIO_FLOAT 0
#define ACKCONF_WANT_EMULATED_TIME 1
#define ACKCONF_WANT_EMULATED_POPEN 0
#define ACKCONF_WANT_EMULATED_FILE 1

/* We have a very small address space, so override the default buffer size. */

#define BUFSIZ 256

#endif
