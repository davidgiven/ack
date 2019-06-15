#ifndef ACK_FILE_H
#define ACK_FILE_H

/* CP/M custom FILE* implementation. */

#define L_tmpnam 16 /* 0a:12345678.abc */
#define ACK_TMP_PREFIX "0a:tmp"
#define TMP_MAX 999

#include <cpm.h>

struct FILE
{
    uint8_t user;
    FCB fcb;
    char buffer[128];
};

#define FOPEN_MAX 8
extern FILE* __files[FOPEN_MAX];

#endif
