_StringAssign(dstsiz, srcsiz, dstaddr, srcaddr)
	register char *dstaddr, *srcaddr;
{
	while (srcsiz > 0) {
		*dstaddr++ = *srcaddr++;
		srcsiz--;
		dstsiz--;
	}
	while (dstsiz > 0) {
		*dstaddr++ = 0;
		dstsiz--;
	}
}
