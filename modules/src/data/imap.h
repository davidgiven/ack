#ifndef IMAP_H
#define IMAP_H

struct imap_node
{
	int left;
	void* right;
};

/* Danger, Will Robinson! The type and the macro must be compatible. */

struct imap
{
	struct imap_node* item;
	int count;
	int max;
};

#define IMAPOF(RIGHT) \
	struct { \
		struct { int left; RIGHT* right; }* item; \
		int count; \
		int max; \
	}

extern void imap_put(void* map, int left, void* right);
extern void imap_add(void* map, int left, void* right);

#endif

