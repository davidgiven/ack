int
_dup2(oldfd, newfd)
{
	_close(newfd);
	return _fcntl(oldfd, 0, newfd);
}
