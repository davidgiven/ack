/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef __UTIL_H_INCLUDED__
#define __UTIL_H_INCLUDED__

/** Raises a warning with the specified message. */
void warning(char* str);
/** Raises a non fatal error with the specified message. */
void error(char* str);
/** Raises a fatal error with the specified message and
    quits the application. */
void fatal(char* str);
/** Error management to raise a warning on an unimplemented
 *  feature. */
void notyetimpl(void);
/** Error management to raise a warning on an illegal command
    line option. */
void illegalcmd(void);


char *myitoa(int i);

/** Tries to allocates a memory block of the specified number of bytes, and exits
 *  with a fatal error upon a failure. Upon allocation success, fills the allocated
 *  memory block with binary 0.
 */
char *salloc(unsigned int length);

#endif /* __UTIL_H_INCLUDED__ */
