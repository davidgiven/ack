/*  getcwd - get current working directory	Author: Terrence W. Holm */

/* Directly derived from Adri Koppes' pwd(1).
 * Modified by Andy Tanenbaum for POSIX (29 Oct. 1989)
 */

#include <lib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/dir.h>
#include <string.h>
#define getcwd	_getcwd

#define  DIRECT_SIZE  (sizeof (struct direct))

PRIVATE _PROTOTYPE(void  go_back, (char *path) );

char *getcwd(buffer, size)
char *buffer;
int size;
/* Get current working directory. */
{
  int same_device, found, fd;
  char *r, path[PATH_MAX + 1], temp_name[NAME_MAX + 1];
  struct stat current, parent, dir_entry;
  struct direct d;

  if (buffer == (char *)NULL || size <= 0) {
	errno = EINVAL;
	return((char *)NULL);
  }
  path[0] = '\0';

  /* Get the inode for the current directory  */
  if (stat(".", &current) == -1) return((char *)NULL);
  if ((current.st_mode & S_IFMT) != S_IFDIR) return((char *)NULL);

  /* Run backwards up the directory tree, grabbing dir names on the way. */
  while (1) {
	same_device = 0;
	found = 0;

	/* Get the inode for the parent directory  */
	if (chdir("..") == -1) return((char *)NULL);
	if (stat(".", &parent) == -1) return((char *)NULL);
	if ((parent.st_mode & S_IFMT) != S_IFDIR) return((char *)NULL);
	if (current.st_dev == parent.st_dev) same_device = 1;

	/* At the root, "." is the same as ".."  */
	if (same_device && current.st_ino == parent.st_ino) break;

	/* Search the parent directory for the current entry  */
	if ((fd = open(".", O_RDONLY)) == -1) return((char *)NULL);
	while (!found && read(fd, (char *)&d, DIRECT_SIZE) == DIRECT_SIZE) {
		if (d.d_ino == 0L) continue;	/* empty slot */
		if (same_device) {
			if (current.st_ino == d.d_ino) found = 1;
		} else {
			temp_name[0] = '\0';
			strncat(temp_name, d.d_name, NAME_MAX);
			if (stat(temp_name, &dir_entry) == -1) {
				close(fd);
				go_back(path);
				return((char *)NULL);
			}
			if (current.st_dev == dir_entry.st_dev &&
			    current.st_ino == dir_entry.st_ino)
				found = 1;
		}
	}

	close(fd);
	if (!found) {
		go_back(path);
		return((char *)NULL);
	}
	if (strlen(path) + NAME_MAX + 1 > PATH_MAX) {
		errno = ERANGE;
		go_back(path);
		return((char *)NULL);
	}
	strcat(path, "/");
	strncat(path, d.d_name, NAME_MAX);
	current.st_dev = parent.st_dev;
	current.st_ino = parent.st_ino;
  }

  /* Copy the reversed path name into <buffer>  */
  if (strlen(path) + 1 > size) {
	errno = ERANGE;
	go_back(path);
	return((char *)NULL);
  }
  if (strlen(path) == 0) {
	strcpy(buffer, "/");
	return(buffer);
  }
  *buffer = '\0';
  while ((r = strrchr(path, '/')) != (char *)NULL) {
	strcat(buffer, r);
	*r = '\0';
  }
  return(chdir(buffer) ? (char *)NULL : buffer);
}

PRIVATE void go_back(path)
char *path;
{
/* If getcwd() gets in trouble and can't complete normally, reverse the
 * path built so far and change there so we end up in the directory that
 * we started in.
 */

  char *r;

  while ((r = strrchr(path, '/')) != (char *)NULL) {
	chdir(r+1);
	*r = '\0';
  }
}
