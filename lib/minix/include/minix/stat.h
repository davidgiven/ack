struct stat {
    short int st_dev;
    unsigned short st_ino;
    unsigned short st_mode;
    short int st_nlink;
    short int st_uid;
    short int st_gid;
    short int st_rdev;
    long st_size;
    long st_atime;
    long st_mtime;
    long st_ctime;
};

/* Some common definitions. */
#define S_IFMT  0170000		/* type of file */
#define S_IFDIR 0040000  	/* directory */
#define S_IFCHR 0020000		/* character special */
#define S_IFBLK 0060000		/* block special */
#define S_IFREG 0100000		/* regular */
#define S_ISUID   04000		/* set user id on execution */
#define S_ISGID   02000		/* set group id on execution */
#define S_ISVTX   01000		/* save swapped text even after use */
#define S_IREAD   00400		/* read permission, owner */
#define S_IWRITE  00200		/* write permission, owner */
#define S_IEXEC   00100		/* execute/search permission, owner */
