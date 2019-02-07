#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include "globals.h"

#define logf(args...) while (0)
//#define logf(args...) printf(args)

struct file
{
	struct file* prev;
	struct file* next;
	cpm_filename_t filename;
	int fd;
	int flags;
};

#define NUM_FILES 16
static struct file files[NUM_FILES];
static struct file* firstfile;

#define NUM_DRIVES 16
static int drives[NUM_DRIVES];

static cpm_filename_t currentpattern;
static int currentsearchdrivefd;
static DIR* currentdir;

void files_init(void)
{
	for (int i=0; i<NUM_DRIVES; i++)
		drives[i] = -1;
	file_set_drive(0, ".");

	for (int i=0; i<NUM_FILES; i++)
	{
		struct file* f = &files[i];
		if (i == 0)
			f->prev = NULL;
		else
			f->prev = &files[i-1];

		if (i == (NUM_FILES-1))
			f->next = NULL;
		else
			f->next = &files[i+1];

		memset(&f->filename.bytes, ' ', 11);
		f->filename.drive = 0;
		f->fd = -1;
		f->flags = 0;
	}

	firstfile = &files[0];
}

void file_set_drive(int drive, const char* path)
{
	if ((drive < 0) || (drive >= NUM_DRIVES))
		fatal("bad drive letter");
		
	if (drives[drive] != -1)
		close(drives[drive]);
	drives[drive] = open(path, O_RDONLY);
	if (drives[drive] == -1)
		fatal("could not open '%s': %s", path, strerror(errno));

	struct stat st;
	fstat(drives[drive], &st);
	if (!S_ISDIR(st.st_mode))
		fatal("could not open '%s': not a directory", path);
	logf("[drive %c now pointing at %s (fd %d)]\n", drive+'A', path, drives[drive]);
}

static void bump(struct file* f)
{
	// logf("[bumping file %d to front]\n", f-files);

	if (f != firstfile)
	{
		/* Remove from list. */
		if (f->prev)
			f->prev->next = f->next;
		if (f->next)
			f->next->prev = f->prev;

		/* Reinsert at head of list. */
		firstfile->prev = f;
		f->prev = NULL;
		f->next = firstfile;
		firstfile = f;
	}

	// logf("[first file is %d]\n", firstfile-files);
	// for (int i=0; i<NUM_FILES; i++)
	// {
	// 	f = &files[i];
	// 	logf("[file %02d: %c:%.11s, fd=%d, prev=%d next=%d]\n",
	// 		i, 'A'-1+f->filename.drive, f->filename.bytes, f->fd,
	// 		f->prev ? (f->prev - files) : -1,
	// 		f->next ? (f->next - files) : -1);
	// }
}

static void cpm_filename_to_unix(cpm_filename_t* cpmfilename, char* unixfilename)
{
	char* pin = cpmfilename->bytes;
	char* pout = unixfilename;

	for (int i=0; i<8; i++)
	{
		char c = *pin++;
		if (c != ' ')
			*pout++ = tolower(c);
	}
	*pout++ = '.';
	for (int i=0; i<3; i++)
	{
		char c = *pin++;
		if (c != ' ')
			*pout++ = tolower(c);
	}
	if (pout[-1] == '.')
		pout--;
	*pout = '\0';
}

static bool unix_filename_to_cpm(const char* unixfilename, cpm_filename_t* cpmfilename)
{
	const char* pin = unixfilename;

	memset(cpmfilename, ' ', sizeof(cpm_filename_t));
	char* pout = &cpmfilename->bytes[0];
	int count = 0;
	int maxcount = 8;
	for (;;)
	{
		char c = *pin++;
		if ((c == '.') && (maxcount == 8))
		{
			maxcount = 3;
			count = 0;
			pout = &cpmfilename->bytes[8];
		}
		else if (c == '\0')
			break;
		else if (count == maxcount)
			return false;
		else if (isupper(c))
			return false;
		else
		{
			*pout++ = toupper(c);
			count++;
		}
	}

	return true;
}

static bool match_filenames(cpm_filename_t* pattern, cpm_filename_t* filename)
{
	if (pattern->drive != filename->drive)
		return false;

	for (int i=0; i<sizeof(pattern->bytes); i++)
	{
		char p = pattern->bytes[i];
		if (p == '?')
			continue;
		if (p != filename->bytes[i])
			return false;
	}
	return true;
}

static int get_drive_fd(cpm_filename_t* filename)
{
	int drive = filename->drive - 1;
	if ((drive < 0) || (drive >= NUM_DRIVES))
	{
		logf("[reference to bad drive %c]\n", drive + 'A');
		return -1;
	}
	int drivefd = drives[drive];
	if (drivefd == -1)
	{
		logf("[reference to undefined drive %c]\n", drive + 'A');
		return -1;
	}
	logf("[selecting drive %c on fd %d]\n", drive + 'A', drivefd);
	return drivefd;
}

static void reopen(struct file* f, int flags)
{
	if ((f->fd == -1) || ((f->flags == O_RDONLY) && (flags == O_RDWR)))
	{
		char unixfilename[13];
		cpm_filename_to_unix(&f->filename, unixfilename);

		if (f->fd != -1)
		{
			logf("[reopening actual file '%s' on %d with different flags]\n", unixfilename, f->fd);
			close(f->fd);
		}

		int drivefd = get_drive_fd(&f->filename);
		if (drivefd == -1)
			return;

		f->flags = flags & O_ACCMODE;
		errno = 0;
		f->fd = openat(drivefd, unixfilename, flags, 0666);
		logf("[opened actual file '%s' to fd %d: %s]\n", unixfilename, f->fd, strerror(errno));
	}

}

static struct file* find_file(cpm_filename_t* filename)
{
	struct file* f = firstfile;
	for (;;)
	{
		if (memcmp(filename, &f->filename, sizeof(cpm_filename_t)) == 0)
			break;

		if (f->next)
			f = f->next;
		else
		{
			logf("[allocating file %d for '%.11s']\n", f-files, filename->bytes);
			bump(f);
			if (f->fd != -1)
			{
				logf("[closing old file %d for '%.11s']\n", f-files, f->filename.bytes);
				close(f->fd);
			}
			f->fd = -1;
			f->filename = *filename;
			f->flags = 0;
			break;
		}
	}
	return f;
}

struct file* file_open(cpm_filename_t* filename)
{
	struct file* f = find_file(filename);
	reopen(f, O_RDONLY);
	if (f->fd == -1)
		return NULL;
	return f;
}

struct file* file_create(cpm_filename_t* filename)
{
	struct file* f = find_file(filename);
	logf("[creating file %d for '%.11s']\n", f-files, f->filename.bytes);
	reopen(f, O_RDWR | O_CREAT);
	if (f->fd == -1)
		return NULL;
	return f;
}

int file_close(cpm_filename_t* filename)
{
	struct file* f = find_file(filename);

	logf("[explicitly closing file %d for '%.11s']\n", f-files, f->filename.bytes);
	if (f->fd != -1)
	{
		logf("[closing file descriptor %d]\n", f->fd);
		close(f->fd);
	}

	memset(&f->filename.bytes, ' ', 11);
	f->fd = -1;
	f->flags = 0;

	return 0;
}

int file_read(struct file* f, uint8_t* data, uint16_t record)
{
	reopen(f, O_RDONLY);
	
	logf("[read record %04x from file %d for '%.11s']\n", record, f-files, f->filename.bytes);
	bump(f);
	memset(data, '\0', 128);
	return pread(f->fd, data, 128, record*128);
}

int file_write(struct file* f, uint8_t* data, uint16_t record)
{
	reopen(f, O_RDWR);

	logf("[write record %04x from file %d for '%.11s']\n", record, f-files, f->filename.bytes);
	bump(f);
	return pwrite(f->fd, data, 128, record*128);
}

int file_getrecordcount(struct file* f)
{
	reopen(f, O_RDONLY);
	
	struct stat st;
	fstat(f->fd, &st);
	return (st.st_size + 127) >> 7;
}

void file_setrecordcount(struct file* f, int count)
{
	reopen(f, O_RDONLY);
	
	if (count != file_getrecordcount(f))
	{
		logf("[truncating file %d to %d records]\n", f-files, count);
		reopen(f, O_RDWR);
		ftruncate(f->fd, count*128);
	}
}

int file_findfirst(cpm_filename_t* pattern)
{
	if (currentdir)
	{
		closedir(currentdir);
		currentdir = NULL;
	}

	currentpattern = *pattern;
	logf("[reset search; current find pattern is '%.11s']\n", currentpattern.bytes);
	currentsearchdrivefd = get_drive_fd(pattern);
	if (currentsearchdrivefd == -1)
		return 0;

	currentdir = fdopendir(dup(currentsearchdrivefd));
	if (currentdir)
	{
		rewinddir(currentdir);
		return 0;
	}
	return -1;
}

int file_findnext(cpm_filename_t* result)
{
	for (;;)
	{
		if (!currentdir)
			return -1;

		struct dirent* de = readdir(currentdir);
		if (!de)
		{
			closedir(currentdir);
			currentdir = NULL;
			logf("[finished search]\n");
			return -1;
		}

		struct stat st;
		if ((fstatat(currentsearchdrivefd, de->d_name, &st, 0) == 0)
			&& S_ISREG(st.st_mode)
			&& unix_filename_to_cpm(de->d_name, result))
		{
			result->drive = currentpattern.drive;
			logf("[compare '%.11s' with pattern '%.11s']\n", result->bytes, currentpattern.bytes);
			if (match_filenames(&currentpattern, result))
			{
				logf("[positive match]\n");
				return 0;
			}
		}
	}
}

int file_delete(cpm_filename_t* pattern)
{
	logf("[attempting to delete pattern '%.11s' on drive %c]\n", pattern->bytes, '@'+pattern->drive);
	int drivefd = get_drive_fd(pattern);
	DIR* dir = fdopendir(dup(drivefd));
	if (!dir)
		return -1;
	rewinddir(dir);

	int result = -1;
	for (;;)
	{
		struct dirent* de = readdir(dir);
		if (!de)
			break;

		struct stat st;
		cpm_filename_t candidate;

		if ((fstatat(drivefd, de->d_name, &st, 0) == 0)
			&& S_ISREG(st.st_mode)
			&& unix_filename_to_cpm(de->d_name, &candidate))
		{
			candidate.drive = pattern->drive;
			logf("[compare '%.11s' with pattern '%.11s']\n", candidate.bytes, pattern->bytes);
			if (match_filenames(pattern, &candidate))
			{
				logf("[positive match, deleting]\n");
				unlinkat(drivefd, de->d_name, 0);
				result = 0;
			}
		}
	}

	closedir(dir);
	return result;
}

int file_rename(cpm_filename_t* src, cpm_filename_t* dest)
{
	logf("[renaming %.11s to %.11s on drive %c]\n",
		src->bytes, dest->bytes, '@'+src->drive);

	char srcunixfilename[13];
	cpm_filename_to_unix(src, srcunixfilename);

	char destunixfilename[13];
	cpm_filename_to_unix(dest, destunixfilename);

	int drivefd = get_drive_fd(src);
	return renameat(drivefd, srcunixfilename, drivefd, destunixfilename);
}