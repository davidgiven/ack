#include <stdio.h>
#include <stdlib.h>
#include <cpm.h>
#include <string.h>
#include <ctype.h>
#include "cpmsys.h"

static const char* fill(uint8_t* dest, const char* src, int len)
{
    do
    {
        char c = toupper(*src);
        if (!c || (c == '.'))
            c = ' ';
        else if (c == '*')
            c = '?';
        else
            src++;
        *dest++ = c;
    }
    while (--len);
    return src;
}

uint8_t cpm_parse_filename(FCB* fcb, const char* filename)
{
    uint8_t user = cpm_get_user();
    char c;

    memset(fcb, 0, sizeof(FCB));
    memset(fcb->f, ' ', sizeof(fcb->f));

    {
        const char* colon = strchr(filename, ':');
        if (colon)
        {
            c = *filename++;
            if (isdigit(c))
            {
                user = strtol(filename-1, (char**) &filename, 10);
                c = *filename++;
            }
            c = toupper(c);
            if (isalpha(c))
            {
                fcb->dr = c - '@';
                c = *filename++;
            }

            filename = colon + 1;
        }
    }

    /* Read filename part. */

    filename = fill(&fcb->f[0], filename, 8);
    filename = strchr(filename, '.');
    if (filename)
        fill(&fcb->f[8], filename+1, 3);

    if (fcb->dr == 0)
        fcb->dr = cpm_get_current_drive() + 1;
    return user;
}

