.extern _errno
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .data
_errno:
.extern _lseek
.short	0
.sect .text
_lseek:
link	a6,#-0
.sect .data
_13:
.short	29561
.short	29489
.short	11875
.short	0
.sect .text
move.l #_13,.filn
move.w #8,.lino
move.w #0,-(sp)
move.w #0,-(sp)
move.l (sp)+,d0
unlk a6
rts
.extern _close
_close:
link	a6,#-0
move.l #_13,.filn
move.w #12,.lino
move.w #0,-(sp)
move.w (sp)+,d0
unlk a6
rts
.extern _ioctl
_ioctl:
link	a6,#-0
move.l #_13,.filn
move.w #18,.lino
move.w #0,-(sp)
move.w (sp)+,d0
unlk a6
rts
.extern _read
_read:
link	a6,#-0
move.l #_13,.filn
move.w #24,.lino
move.w #-1,-(sp)
move.w (sp)+,d0
unlk a6
rts
.extern _open
_open:
link	a6,#-0
move.l #_13,.filn
move.w #30,.lino
move.w #-1,-(sp)
move.w (sp)+,d0
unlk a6
rts
