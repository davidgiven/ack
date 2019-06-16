/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <cpm.h>
#include "cpmsys.h"

void _sys_write_tty(char c)
{
	cpm_conout(c);
	if (c == '\n')
		cpm_conout(c);
}

ssize_t write(int fd, void* buffer, size_t count)
{
	uint8_t* bbuffer = buffer;
	struct FCBE* fcbe = &__fd[fd];
	uint8_t olduser;
	uint16_t result;

    __init_file_descriptors();
	if (fcbe->fcb.dr == 0)
	{
		/* Write to the console. */

		size_t i = count;
		while (i--)
			_sys_write_tty(*bbuffer++);
		return count;
	}

	olduser = cpm_get_user();
	cpm_set_user(fcbe->user);

	if (fcbe->offset || !SECTOR_ALIGNED(count))
	{
		uint8_t delta;

		/* We're not at a sector boundary, so we need to do a
		 * read/modify/write of the initial fragment. */

		cpm_set_dma(__transfer_buffer);
		if (cpm_read_random_safe(&fcbe->fcb) != 0)
			goto eio;

		/* Copy enough bytes to reach the end of the sector. */

		delta = 128 - fcbe->offset;
		if (delta > count)
			delta = count;
		memcpy(__transfer_buffer+fcbe->offset, bbuffer, delta);
		fcbe->offset += delta;
		count -= delta;
		bbuffer += delta;

		/* Write back. */

		if (cpm_write_random(&fcbe->fcb) != 0)
			goto eio;

		/* If we've written enough bytes, advance to the next sector. */

		if (fcbe->offset == 128)
		{
			U16(fcbe->fcb.r)++;
			fcbe->offset = 0;
		}
	}

	while (count >= 128)
	{
		/* Write entire sectors directly from the source buffer. */

		cpm_set_dma(bbuffer);
		if (cpm_write_random(&fcbe->fcb) != 0)
			goto eio;
		count -= 128;
		bbuffer += 128;
		U16(fcbe->fcb.r)++;
	}

	if (count != 0)
	{
		/* There's some trailing data to write. We need another
		 * read/modify/write cycle. */

		cpm_set_dma(__transfer_buffer);
		if (cpm_read_random_safe(&fcbe->fcb) != 0)
			goto eio;

		memcpy(__transfer_buffer, bbuffer, count);

		if (cpm_write_random(&fcbe->fcb) != 0)
			goto eio;

		fcbe->offset = count;
	}

	if (U16(fcbe->fcb.r) >= fcbe->length)
	{
		fcbe->length = U16(fcbe->fcb.r);
		if (fcbe->offset != 0)
			fcbe->length++;
	}

	result = bbuffer - (uint8_t*)buffer;
exit:
	cpm_set_user(olduser);
	return result;

eio:
	errno = EIO;
	result = -1;
	goto exit;
}
