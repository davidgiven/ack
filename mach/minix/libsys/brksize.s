.define _brksize
.sect .text; .sect .rom; .sect .data
.extern endbss, _brksize
.sect .data
_brksize: .data2 endbss
