#

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

    rlwinm r6, r4, 29, 3, 29 /* r6 = byte index of word in set */
    extrwi r7, r4, 5, 27     /* r7 = bit number within word */

    lwzx r8, r5, r6     /* r8 = individual word from set */
    srw r8, r8, r7
    extrwi r8, r8, 1, 31

    addi sp, sp, 8      /* retract over the two words */
    add sp, sp, r3      /* retract over bitfield */
    stwu r8, -4(sp)     /* push result */
    blr                 /* return */
