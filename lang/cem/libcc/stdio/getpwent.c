/*
 * get entry from password file
 *
 * By Patrick van Kleef
 *
 */


#include <pwd.h>

#define PRIVATE static


PRIVATE char  _pw_file[] = "/etc/passwd";
PRIVATE char  _pwbuf[256];
PRIVATE char  _buffer[1024];
PRIVATE char *_pnt;
PRIVATE char *_buf;
PRIVATE int   _pw = -1;
PRIVATE int   _bufcnt;
PRIVATE struct passwd    pwd;

setpwent() 
{
	if (_pw >= 0)
		lseek (_pw, 0L, 0);
	else
		_pw = open (_pw_file, 0);

	_bufcnt = 0;
	return (_pw);
}


endpwent () 
{
	if (_pw >= 0)
		close (_pw);

	_pw = -1;
	_bufcnt = 0;
}

static getline () 
{
	if (_pw < 0 && setpwent () < 0)
		return (0);
	_buf = _pwbuf;
	do {
		if (--_bufcnt <= 0){
			if ((_bufcnt = read (_pw, _buffer, 1024)) <= 0)
				return (0);
			else
				_pnt = _buffer;
		}
		*_buf++ = *_pnt++;
	} while (*_pnt != '\n');
	_pnt++;
	_bufcnt--;
	*_buf = 0;
	_buf = _pwbuf;
	return (1);
}

static skip_period () 
{
	while (*_buf != ':')
		_buf++;

	*_buf++ = '\0';
}

struct passwd  *getpwent () 
{
	if (getline () == 0)
		return (0);

	pwd.pw_name = _buf;
	skip_period ();
	pwd.pw_passwd = _buf;
	skip_period ();
	pwd.pw_uid = atoi (_buf);
	skip_period ();
	pwd.pw_gid = atoi (_buf);
	skip_period ();
	pwd.pw_gecos = _buf;
	skip_period ();
	pwd.pw_dir = _buf;
	skip_period ();
	pwd.pw_shell = _buf;

	return (&pwd);
}

struct passwd  *getpwnam (name)
char   *name;
{
	struct passwd  *pwd;

	setpwent ();
	while ((pwd = getpwent ()) != 0)
		if (!strcmp (pwd -> pw_name, name))
			break;
	endpwent ();
	if (pwd != 0)
		return (pwd);
	else
		return (0);
}

struct passwd  *getpwuid (uid)
int     uid;
{
	struct passwd  *pwd;

	setpwent ();
	while ((pwd = getpwent ()) != 0)
		if (pwd -> pw_uid == uid)
			break;
	endpwent ();
	if (pwd != 0)
		return (pwd);
	else
		return (0);
}
