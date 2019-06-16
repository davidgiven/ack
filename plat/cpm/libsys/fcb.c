#include <stdio.h>
#include <cpm.h>
#include <string.h>
#include <ctype.h>
#include "cpmsys.h"

uint8_t cpm_parse_filename(FCB* fcb, const char* filename)
{
    uint8_t user = cpm_get_user();

    memset(fcb, 0, sizeof(FCB));
    memset(fcb->f, ' ', sizeof(fcb->f));

    if (strchr(filename, ':'))
    {
        char c = *filename++;
        if (isdigit(c))
        {
            user = c - '0';
            c = *filename++;
            if (isdigit(c))
            {
                user = (user*10) + (c - '0');
                c = *filename++;
            }
        }
        c = toupper(c);
        if (isalpha(c))
        {
            fcb->dr = c - '@';
            c = *filename++;
        }

        while (c != ':')
            c = *filename++;
    }

    /* Read filename part. */

    {
        uint8_t i = 8;
        uint8_t* p = &fcb->f[0];
        while (i--)
        {
            char c = toupper(*filename++);
            if (c == '.')
                break;
            if (!c)
                goto exit;
            *p++ = c;
        }
    }

    /* Read extension part. */

    {
        uint8_t i = 3;
        uint8_t* p = &fcb->f[8];
        while (i--)
        {
            char c = toupper(*filename++);
            if (!c)
                goto exit;
            *p++ = c;
        }
    }

exit:
    if (fcb->dr == 0)
        fcb->dr = cpm_get_current_drive() + 1;
    return user;
}

