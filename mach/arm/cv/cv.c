#include <stdio.h>
#include <out.h>
#include "arm.h"
struct outhead ohead;
struct outsect sect[NSECT];
struct armhead ahead;
struct chunkhead chead;
struct areadec area[NSECT];
int fd;


main (argc,argv)
int argc;
char *argv[];
{
        if (argc != 3)   fatal ("usage: dl ackobj armobj\n");
        if (! rd_open (*++argv))
                fatal ("can't open \n");
	if ((fd = open(*++argv, 1)) <0)
                fatal ("can't open \n");
        else    {
                convert ();
                }
}

convert ()
{
        int i,areasize=0;
	char buf[4];
	int cnt;

        rd_ohead(&ohead);
        if (ohead.oh_flags & HF_LINK) {
                fatal("unresolved references\n");
        }
        ahead.chunkfileid = 0xC3CBC6C5;
        ahead.maxchunks = MAXCHUNK;
        ahead.numchunks = MAXCHUNK;
	strncpy(ahead.entries[0].chunkid , "OBJ_HEAD", 8);
	strncpy(ahead.entries[1].chunkid , "OBJ_AREA", 8);
	strncpy(ahead.entries[2].chunkid , "OBJ_IDFN", 8);
	strncpy(ahead.entries[3].chunkid , "OBJ_SYMT", 8);
	strncpy(ahead.entries[4].chunkid , "OBJ_STRT", 8);
	ahead.entries[0].offset = sizeof(struct armhead);
	ahead.entries[0].size = sizeof(struct chunkhead) + ohead.oh_nsect *
					sizeof(struct areadec);
	ahead.entries[1].offset = ahead.entries[0].offset+ahead.entries[0].size;
        chead.objtype = 0xC5E2D080;
        chead.versionid = 110;
        chead.narea = ohead.oh_nsect;
        chead.nsymb = ohead.oh_nname;
        chead.entrya = 0;
        chead.entryo = 0;
	lseek(fd, ahead.entries[1].offset, 0);
        rd_sect(sect, ohead.oh_nsect);
        for (i = 0; i < ohead.oh_nsect; i++) {
                area[i].name=0;
                area[i].atal=2|1<<7|(i=0?(1<<8):0);
                area[i].size=sect[i].os_size;
                area[i].nrel=0;
                area[i].base=sect[i].os_base;
		rd_outsect(i);
		cnt = sect[i].os_flen;
		areasize += cnt;
		if (cnt!=sect[i].os_size)
			area[i].atal |= 1<<11;
			if (cnt & 3)
				fatal("alignment error\n");	
			while (cnt){
				rd_emit(buf, (long)4);
				write(fd, buf, 4);
				cnt -= 4;
			}
        }
	ahead.entries[1].size = areasize;
	lseek(fd, 0, 0);
	write(fd, &ahead, sizeof(struct armhead));
	write(fd, &chead, sizeof(struct chunkhead));
	for(i=1; i<NSECT; i++)
		write(fd, &area[i], sizeof(struct areadec));
	close(fd);
}

fatal(s)
char *s;
{
        printf (s);
        exit (-1);
}

rd_fatal(s)
char *s;
{
        printf (s);
        exit (-1);
}
