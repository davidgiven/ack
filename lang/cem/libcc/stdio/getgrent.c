/*
 * get entry from group file
 * 
 * By: Patrick van Kleef
 */

#include <grp.h>

#define PRIVATE static

PRIVATE char _gr_file[] = "/etc/group";
PRIVATE char _grbuf[256];
PRIVATE char _buffer[1024];
PRIVATE char *_pnt;
PRIVATE char *_buf;
PRIVATE int  _gfd = -1;
PRIVATE int  _bufcnt;
PRIVATE struct group grp;

setgrent ()
{
        if (_gfd >= 0)
	        lseek (_gfd, 0L, 0);
        else
	        _gfd = open (_gr_file, 0);

        _bufcnt = 0;
        return (_gfd);
}


endgrent () 
{
        if (_gfd >= 0)
	        close (_gfd);

        _gfd = -1;
        _bufcnt = 0;
}


static getline () 
{
        if (_gfd < 0 && setgrent () < 0)
	        return (0);

        _buf = _grbuf;
        do {
	        if (--_bufcnt <= 0){
	                if ((_bufcnt = read (_gfd, _buffer, 1024)) <= 0)
		                return (0);
	                else
		                _pnt = _buffer;
		}
	        *_buf++ = *_pnt++;
        } while (*_pnt != '\n');
        _pnt++;
        _bufcnt--;
        *_buf = 0;
        _buf = _grbuf;
        return (1);
}

static skip_period () 
{
        while (*_buf != ':')
	        _buf++;
        *_buf++ = '\0';
}

struct group   *getgrent () 
{
        if (getline () == 0)
               return (0);

        grp.gr_name = _buf;
        skip_period ();
        grp.gr_passwd = _buf;
        skip_period ();
        grp.gr_gid = atoi (_buf);
        skip_period ();
        return (&grp);
}

struct group   *getgrnam (name)
char   *name;
{
        struct group *grp;

        setgrent ();
        while ((grp = getgrent ()) != 0)
	        if (!strcmp (grp -> gr_name, name))
	                break;
        endgrent ();
        if (grp != 0)
	        return (grp);
        else
	        return (0);
}

struct group   *getgrgid (gid)
int     gid;
{
        struct group   *grp;

        setgrent ();
        while ((grp = getgrent ()) != 0)
	        if (grp -> gr_gid == gid)
	                break;
        endgrent ();
        if (grp != 0)
	        return (grp);
        else
	        return (0);
}
