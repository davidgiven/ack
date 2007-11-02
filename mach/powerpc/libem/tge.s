#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"

.sect .rom

! Lookup table for tge.

.define .teq_table
.teq_table:
	.data4 1       ! . .
	.data4 0       ! . G
	.data4 0       ! L .

.define .tne_table
.tne_table:
	.data4 0       ! . .
	.data4 1       ! . G
	.data4 1       ! L .

.define .tgt_table
.tgt_table:
	.data4 0       ! . .
	.data4 1       ! . G
	.data4 0       ! L .

.define .tge_table
.tge_table:
	.data4 1       ! . .
	.data4 1       ! . G
	.data4 0       ! L .

.define .tlt_table
.tlt_table:
	.data4 0       ! . .
	.data4 0       ! . G
	.data4 1       ! L .

.define .tle_table
.tle_table:
	.data4 1       ! . .
	.data4 0       ! . G
	.data4 1       ! L .
