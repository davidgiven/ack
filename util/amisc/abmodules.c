#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "out.h"
#include "arch.h"
#include "ranlib.h"
#include "object.h"
#include "diagnostics.h"
#include "stringlist.h"

int	numsort_flg;
int	sectsort_flg;
int	undef_flg;
int	revsort_flg = 1;
int	globl_flg;
int	nosort_flg;
int	arch_flg;
int	prep_flg;
int	read_error;
struct	outsect	sbuf;
long	off;
long	s_base[S_MAX];	/* for specially encoded bases */
char	*filename;
int	narg;


static const char prefix[] = "_bmodule_";

static struct stringlist modules;

static void do_file(FILE *fd)
{
	struct outhead hbuf;
	char		*cbufp;
	long		fi_to_co;
	long		n;
	unsigned	readcount;

	read_error = 0;
	rd_fdopen(fd);

	rd_ohead(&hbuf);
	if (BADMAGIC(hbuf))
		return;

	n = hbuf.oh_nname;
	if (n == 0)
		fatal("%s --- no name list", filename);

	if (hbuf.oh_nchar == 0)
		fatal("%s --- no names", filename);

	if ((readcount = hbuf.oh_nchar) != hbuf.oh_nchar)
		fatal("%s --- string area too big", filename);

	cbufp = calloc(readcount, 1);
	rd_string(cbufp, hbuf.oh_nchar);
	if (read_error)
		goto corrupt;

	fi_to_co = (long) (cbufp - OFF_CHAR(hbuf));
	while (--n >= 0)
	{
		struct outname nbuf;

		rd_name(&nbuf, 1);
		if (read_error)
			goto corrupt;

		if (!(nbuf.on_type & S_EXT))
			continue;
		if ((nbuf.on_type & S_TYP) == S_UND)
			continue;

		if (nbuf.on_foff == 0)
			nbuf.on_mptr = 0;
		else
			nbuf.on_mptr = (char *) (nbuf.on_foff + fi_to_co);

		if (strlen(nbuf.on_mptr) <= sizeof(prefix))
			continue;
		if (memcmp(nbuf.on_mptr, prefix, sizeof(prefix)-1) != 0)
			continue;

		stringlist_add(&modules, strdup(nbuf.on_mptr + sizeof(prefix) - 1));
	}

	if (cbufp)
		free(cbufp);

	return;
corrupt:
	fatal("%s --- corrupt", filename);
}

static void process(FILE* fd)
{
	uint16_t magic = rd_unsigned2(fd);
	switch(magic) {
		case O_MAGIC:
			fseek(fd, 0L, SEEK_SET);
			do_file(fd);
			break;

		case ARMAG:
		case AALMAG:
		{
			struct ar_hdr archive_header;
			static char	buf[sizeof(archive_header.ar_name)+1];

			while (rd_arhdr(fd, &archive_header))
			{
				long nextpos = ftell(fd) + archive_header.ar_size;
				if (nextpos & 1)
					nextpos++;

				strncpy(buf, archive_header.ar_name, sizeof(archive_header.ar_name));
				filename = buf;
				if (strcmp(filename, SYMDEF) != 0)
					do_file(fd);
				fseek(fd, nextpos, SEEK_SET);
			}
			break;
		}
		
		default:
			fatal("file %s is of unknown format", filename);
	}
}

int main(int argc, char* const argv[])
{
	FILE* outputfp = NULL;

	program_name = argv[0];
	for (;;)
	{
		int opt = getopt(argc, argv, "o:");
		if (opt == -1)
			break;

		switch (opt)
		{
			case 'o':
				outputfp = fopen(optarg, "w");
				if (!outputfp)
					fatal("cannot open output file: %s", strerror(errno));
				break;

			default:
				fatal("usage: abmodules [-o outputfile] [file...]");
		}
	}

	for (;;)
	{
		FILE *fd;

		filename = argv[optind++];
		if (!filename)
			break;
		if ((fd = fopen(filename, "rb")) == NULL)
			fatal("cannot open %s: %s", filename, strerror(errno));
		process(fd);
		fclose(fd);
	}

	if (outputfp)
	{
		struct stringfragment* f;

		fprintf(outputfp, "#include <stdint.h>\n");
		fprintf(outputfp, "\n");

		for (f = modules.first; f; f = f->next)
			fprintf(outputfp, "extern uintptr_t bmodule_%s[];\n", f->data);

		fprintf(outputfp, "\n");
		fprintf(outputfp, "extern void patch_addresses(uintptr_t* module);\n");
		fprintf(outputfp, "\n");
		fprintf(outputfp, "void binit(void) {\n");
		for (f = modules.first; f; f = f->next)
			fprintf(outputfp, "\tpatch_addresses(bmodule_%s);\n", f->data);
		fprintf(outputfp, "}\n");
		fclose(outputfp);
	}
	else
	{
		struct stringfragment* f;

		for (f = modules.first; f; f = f->next)
			printf("%s\n", f->data);
	}

	exit(0);
}

void rd_fatal(void)
{
	fatal("read error on %s", filename);
}
