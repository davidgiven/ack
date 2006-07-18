#define MAXCHUNK        5

/* Undefined by dtrg --- seems to be redefined by cv.c?
 * #define NSECT		4
 */


struct entry{
        char chunkid[8];
        long offset;
        long size;
};

struct armhead{
        long chunkfileid;
        long maxchunks;
        long numchunks;
        struct entry entries[MAXCHUNK];
};

struct chunkhead{
        long objtype;
        long versionid;
        long narea;
        long nsymb;
        long entrya;
        long entryo;
};

struct areadec{
        long name;
        long atal;
        long size;
        long nrel;
        long base;
};

