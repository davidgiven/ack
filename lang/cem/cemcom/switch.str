/* $Header$ */
/*		S W I T C H - T A B L E - S T R U C T U R E		*/

struct switch_hdr	{
	struct switch_hdr *next;
	label sh_break;
	label sh_default;
	label sh_table;
	int sh_nrofentries;
	struct type *sh_type;
	arith sh_lowerbd;
	arith sh_upperbd;
	struct case_entry *sh_entries;
};


/* allocation definitions of struct switch_hdr */
/* ALLOCDEF "switch_hdr" */
extern char *st_alloc();
extern struct switch_hdr *h_switch_hdr;
#define	new_switch_hdr() ((struct switch_hdr *) \
		st_alloc((char **)&h_switch_hdr, sizeof(struct switch_hdr)))
#define	free_switch_hdr(p) st_free(p, h_switch_hdr, sizeof(struct switch_hdr))


struct case_entry	{
	struct case_entry *next;
	label ce_label;
	arith ce_value;
};


/* allocation definitions of struct case_entry */
/* ALLOCDEF "case_entry" */
extern char *st_alloc();
extern struct case_entry *h_case_entry;
#define	new_case_entry() ((struct case_entry *) \
		st_alloc((char **)&h_case_entry, sizeof(struct case_entry)))
#define	free_case_entry(p) st_free(p, h_case_entry, sizeof(struct case_entry))

