#include <errno.h>
#include <unistd.h>
#include <cpm.h>
#include "cpmsys.h"

int unlink(const char* path)
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

	if (cpm_delete_file(&fcbe->fcb) == 0xff)
	{
		fcbe->fcb.f[0] = 0;
		errno = EIO;
		return -1;
	}

	return 0;
}
