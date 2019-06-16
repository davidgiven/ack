#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <cpm.h>
#include "cpmsys.h"

int open(const char* path, int access, ...)
{
	uint8_t fd = 0;
    struct FCBE* fcbe = &__fd[0];
	uint8_t olduser;
	
    __init_file_descriptors();
	while (fd != NUM_FILE_DESCRIPTORS)
	{
		if (fcbe->fcb.f[0] == 0)
			break;
		fd++;
		fcbe++;
	}
	if (fd == NUM_FILE_DESCRIPTORS)
	{
		errno = EMFILE;
		return -1;
	}

	fcbe->user = cpm_parse_filename(&fcbe->fcb, path);

	olduser = cpm_get_user();
	cpm_set_user(fcbe->user);

	if (access & O_TRUNC)
	{
		cpm_delete_file(&fcbe->fcb);
		access |= O_CREAT;
	}
	if (access & O_CREAT)
	{
		if (cpm_make_file(&fcbe->fcb) == 0xff)
			goto eio;
	}
	else
	{
		if (cpm_open_file(&fcbe->fcb) == 0xff)
			goto eio;
	}

	cpm_seek_to_end(&fcbe->fcb);
	fcbe->length = U16(fcbe->fcb.r);
	if (!(access & O_APPEND))
		U16(fcbe->fcb.r) = 0;
	return fd;

eio:
	fcbe->fcb.f[0] = 0;
	errno = EIO;
	return -1;
}


