/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	mcomm.c -- change ".lcomm name" into ".comm name" where "name"
	is specified in a list.
*/
#include <stdio.h>

#define IDFSIZE 4096

char *readfile();

struct node {
	char *name;
	struct node *left, *right;
};

char *
Malloc(n)
	unsigned n;
{
	char *space;
	char *malloc();

	if ((space = malloc(n)) == 0) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	return space;
}

struct node *make_tree();

#define new_node() ((struct node *) Malloc(sizeof (struct node)))

main(argc, argv)
	char *argv[];
{
	char *nl_file, *as_file;
	char *nl_text, *as_text;
	struct node *nl_tree = 0;
	int nl_siz, as_siz;

	if (argc != 3) {
		fprintf(stderr, "use: %s namelist assembler_file\n", argv[0]);
		exit(1);
	}
	nl_file = argv[1];
	as_file = argv[2];

	if ((nl_text = readfile(nl_file, &nl_siz)) == 0) {
		fprintf(stderr, "%s: cannot read namelist %s\n",
			argv[0], nl_file);
		exit(1);
	}

	if ((as_text = readfile(as_file, &as_siz)) == 0) {
		fprintf(stderr, "%s: cannot read assembler file %s\n",
			argv[0], as_file);
		exit(1);
	}

	nl_tree = make_tree(nl_text);
	edit(as_text, nl_tree);

	if (writefile(as_file, as_text, as_siz) == 0) {
		fprintf(stderr, "%s: cannot write to %s\n", argv[0], as_file);
		exit(1);
	}
	return 0;
}

#include <sys/types.h>
#include <stat.h>

char *
readfile(filename, psiz)
	char *filename;
	int *psiz;
{
	struct stat stbuf;	/* for `stat' to get filesize		*/
	register int fd;	/* filedescriptor for `filename'	*/
	register char *cbuf;	/* pointer to buffer to be returned	*/

	if (((fd = open(filename, 0)) < 0) || (fstat(fd, &stbuf) != 0))
		return 0;
	cbuf = Malloc(stbuf.st_size + 1);
	if (read(fd, cbuf, stbuf.st_size) != stbuf.st_size)
		return 0;
	cbuf[stbuf.st_size] = '\0';
	close(fd);		/* filedes no longer needed	*/
	*psiz = stbuf.st_size;
	return cbuf;
}

int
writefile(filename, text, size)
	char *filename, *text;
{
	register fd;

	if ((fd = open(filename, 1)) < 0)
		return 0;
	if (write(fd, text, size) != size)
		return 0;
	close(fd);
	return 1;
}

struct node *
make_tree(nl)
	char *nl;
{
	char *id = nl;
	struct node *tree = 0;

	while (*nl) {
		if (*nl == '\n') {
			*nl = '\0';
			insert(&tree, id);
			id = ++nl;
		}
		else {
			++nl;
		}
	}
	return tree;
}

insert(ptree, id)
	struct node **ptree;
	char *id;
{
	register cmp;

	if (*ptree == 0) {
		register struct node *nnode = new_node();

		nnode->name = id;
		nnode->left = nnode->right = 0;
		*ptree = nnode;
	}
	else
	if ((cmp = strcmp((*ptree)->name, id)) < 0)
		insert(&((*ptree)->right), id);
	else
	if (cmp > 0)
		insert(&((*ptree)->left), id);
}

struct node *
find(tree, id)
	struct node *tree;
	char *id;
{
	register cmp;

	if (tree == 0)
		return 0;
	if ((cmp = strcmp(tree->name, id)) < 0)
		return find(tree->right, id);
	if (cmp > 0)
		return find(tree->left, id);
	return tree;
}

edit(text, tree)
	char *text;
	struct node *tree;
{
	register char *ptr = text;
	char idbuf[IDFSIZE];
	register char *id;
	register char *save_ptr;

	while (*ptr) {
		if (
			*ptr   == '.' &&
			*++ptr == 'l' &&
			*++ptr == 'c' &&
			*++ptr == 'o' &&
			*++ptr == 'm' &&
			*++ptr == 'm' &&
			(*++ptr == ' ' || *ptr == '\t')
		)
		{
			save_ptr = ptr - 6;
			while (*++ptr == ' ' || *ptr == '\t')
				;
			if (*ptr == '_')
				++ptr;
			if (InId(*ptr)) {
				id = &idbuf[0];
				*id++ = *ptr++;
				while (InId(*ptr))
					*id++ = *ptr++;
				*id = '\0';
				if (find(tree, idbuf) != 0) {
					*save_ptr++ = ' ';
					*save_ptr++ = '.';
				}
			}
		}
		while (*ptr && *ptr++ != '\n')
			;
	}
}

InId(c)
{
	switch (c) {

	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O':
	case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y':
	case 'Z':
	case '_':
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return 1;
	
	default:
		return 0;
	}
}

puttree(nd)
	struct node *nd;
{
	if (nd) {
		puttree(nd->left);
		printf("%s\n", nd->name);
		puttree(nd->right);
	}
}
