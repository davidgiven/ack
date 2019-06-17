#include <stdio.h>
#include <string.h>
#include <cpm.h>
#include "test.h"

struct testcase
{
    const char* input;
    uint8_t drive;
    uint8_t user;
    const char filename[11];
};

struct testcase tests[] =
{
    { "12345678.ABC", 'A', 0,  "12345678ABC" },
    { "12345678.A",   'A', 0,  "12345678A  " },
    { "12345678",     'A', 0,  "12345678   " },
    { "1",            'A', 0,  "1          " },
    { ".X",           'A', 0,  "        X  " },

    { "B:FOO",        'B', 0,  "FOO        " },
    { "Z:FOO",        'Z', 0,  "FOO        " },
    { "1Z:FOO",       'Z', 1,  "FOO        " },
    { "23Z:FOO",      'Z', 23, "FOO        " },

    { "*.*",          'A', 0,  "???????????" },
    { "FOO*.*",       'A', 0,  "FOO????????" },
    { "FOO*",         'A', 0,  "FOO?????   " },
    { "FOO.*",        'A', 0,  "FOO     ???" },
};

static int failed = 0;

static void do_test(int i, struct testcase* test)
{
    static FCB fcb;
    uint8_t got_user = cpm_parse_filename(&fcb, test->input);

    if ((got_user == test->user)
        && (fcb.dr == (test->drive - '@'))
        && (memcmp(fcb.f, test->filename, 11) == 0))
        return;

    printf("Want: %s -> %d%c:'%.11s' but got %d%c:'%.11s'\n",
        test->input,
        test->user, test->drive, test->filename,
        got_user, fcb.dr+'@', fcb.f);

    failed = 1;
}

int main(int argc, const char* argv[])
{
    int i;

    failed = 0;
    cpm_select_drive(0); /* A: */
    cpm_set_user(0);

    for (i=0; i<sizeof(tests)/sizeof(*tests); i++)
        do_test(i, &tests[i]);

    if (failed)
        fail(0);
    finished();
}
