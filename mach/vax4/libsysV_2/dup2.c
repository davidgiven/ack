int
dup2(oldfd, newfd)
{
	close(newfd);
	return fcntl(oldfd, 0, newfd);
}
