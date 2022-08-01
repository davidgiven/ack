extern int ioctl(int fd, unsigned int request, ...);

int isatty(int fd)
{
    unsigned u;

    if (ioctl(fd, /*TIOCGETD*/(('t'<<8)|0), &u) < 0)
        return 0;
    return 1;
}
