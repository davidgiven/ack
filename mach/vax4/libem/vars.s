.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .data
.define  hol0
.define  .reghp
.define  .trppc
.define  .trpim

hol0:
        .space  8
.trppc:
        .space  4
.trpim:
        .data4   0
.reghp:
        .data4   _end
