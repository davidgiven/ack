char *_syspc;

profil(buff, bufsiz, offset, scale)
	char *buff;
	int (*offset)();
{
	_xprofil(buff, bufsiz, offset, scale, &_syspc);
}
