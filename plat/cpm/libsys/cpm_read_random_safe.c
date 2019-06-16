#include <string.h>
#include <cpm.h>

uint8_t cpm_read_random_safe(FCB* fcb)
{
    uint8_t r = cpm_read_random(fcb);
    if ((r == 1) || (r == 4))
        r = 0;
    return r;
}
