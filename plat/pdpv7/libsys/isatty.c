int isatty(int fd)
{
    char* p;

    if (gtty(fd, &p) < 0)
        return 0;
    return 1;
}
