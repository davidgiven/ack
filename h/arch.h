#define	ARMAG	0177545
#define AALMAG	0177454

struct	ar_hdr {
	char	ar_name[14];
	long	ar_date;
	char	ar_uid;
	char	ar_gid;
	int	ar_mode;
	long	ar_size;
};

#define AR_TOTAL	26
#define AR_SIZE		22
