/* $Source$
 * $State$
 * $Revision$
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <cpm.h>
#include "cpmsys.h"

ssize_t read(int fd, void* buffer, size_t count)
{
	uint8_t* bbuffer = buffer;
	struct FCBE* fcbe = &__fd[fd];
	uint8_t olduser;
	ssize_t result;
	uint8_t* src;

    __init_file_descriptors();
	if (fcbe->fcb.dr == 0)
	{
		/* Read from the console. */

		if (count == 0)
			return 0;
		*(uint8_t*)buffer = cpm_conin();
		return 1;
	}

	olduser = cpm_get_user();
	cpm_set_user(fcbe->user);

	if (U16(fcbe->fcb.r) >= fcbe->length)
		goto done;
	if (fcbe->offset || !SECTOR_ALIGNED(count))
	{
		/* We need to read bytes until we're at a sector boundary. */

		cpm_set_dma(__transfer_buffer);
		if (cpm_read_random_safe(&fcbe->fcb) != 0)
			goto eio;

		/* Copy enough bytes to reach the end of the sector. */

		src = __transfer_buffer + fcbe->offset;
		while ((count != 0) && (fcbe->offset != 128))
		{
			*bbuffer++ = *src++;
			fcbe->offset++;
			count--;
		}

		/* If we've read enough bytes, advance to the next sector. */

		if (fcbe->offset == 128)
		{
			U16(fcbe->fcb.r)++;
			fcbe->offset = 0;
		}
	}

	while (count >= 128)
	{
		if (U16(fcbe->fcb.r) >= fcbe->length)
			goto done;

		/* Read entire sectors directly into the destination buffer. */

		cpm_set_dma(bbuffer);
		if (cpm_read_random_safe(&fcbe->fcb) != 0)
			goto eio;
		count -= 128;
		bbuffer += 128;
		U16(fcbe->fcb.r)++;
	}

	if (count != 0)
	{
		if (U16(fcbe->fcb.r) >= fcbe->length)
			goto done;

		/* There's some trailing data to read. */

		cpm_set_dma(__transfer_buffer);
		if (cpm_read_random_safe(&fcbe->fcb) != 0)
			goto eio;

		src = __transfer_buffer;
		while (count != 0)
		{
			*bbuffer++ = *src++;
			count--;
		}

		fcbe->offset = count;
	}

done:
	result = bbuffer - (uint8_t*)buffer;
exit:
	cpm_set_user(olduser);
	return result;

eio:
	errno = EIO;
	result = -1;
	goto exit;
}
