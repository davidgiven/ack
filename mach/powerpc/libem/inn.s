#include "powerpc.h"

.sect .text

/* Tests a bit in a bitset on the stack.
 *
 * Stack: ( bitset bitnum setsize -- bool )
 */

.define .inn
.inn:
    lwz r3, 0(sp)       /* r3 = size (bytes) */
    lwz r4, 4(sp)       /* r4 = bit number */
    addi r5, sp, 8      /* r5 = base address of bit set */

    srawi r6, r4, 3     /* r6 = byte address into set */
    andi. r7, r4, 7     /* r7 = bit within byte */

    lbzx r8, r5, r6     /* r8 = individual byte from set */
    sraw r8, r8, r7
    rlwinm r8, r8, 0, 31, 31

    addi sp, sp, 8      /* retract over the two words */
    add sp, sp, r3      /* retract over bitfield */
    stwu r8, -4(sp)     /* push result */
    bclr ALWAYS, 0, 0   /* return */
