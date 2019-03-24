/*
 * archive.h
 *
 *  Created on: 2018-11-18
 *      Author: carl
 */

#ifndef __ARCHIVE_H_INCLUDED__
#define __ARCHIVE_H_INCLUDED__

/*
 * Process archive with table of contents. The table of contents tells
 * of symbols in which module they are defined. We scan the table for
 * symbols that are known but not yet defined. Then we extract all necessary
 * information from the corresponding module. This module may need symbols that
 * were defined in modules located before this one in the archive, so we
 * scan the table again. We perform these actions as long as new symbols
 * are defined.
 */
void arch(void);

/*
 * An archive member that will be loaded is remembered by storing its position
 * in the archive into the table of positions.
 */
void notelib(long pos);

/*
 * Process the archive in pass 2.
 * We walk through the table of positions telling at what byte offset the
 * archive header + module is located, until this position is ENDLIB, meaning
 * that we've processed all needed modules in this archive. Each group of
 * positions of an archive is terminated with ENDLIB.
 */
void arch2(void);

#endif /* __ARCHIVE_H_INCLUDED__ */
