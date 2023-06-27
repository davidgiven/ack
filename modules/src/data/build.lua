local hfiles = {
	"./array.h",
	"./astring.h",
	"./buffer.h",
	"./diagnostics.h",
	"./imap.h",
	"./pmap.h",
	"./pset.h",
	"./smap.h",
	"./stringlist.h",
}

clibrary {
	name = "lib",
	srcs = {
		"./array.c",
		"./astring.c",
		"./buffer.c",
		"./diagnostics.c",
		"./imap.c",
		"./pmap.c",
		"./pset.c",
		"./smap.c",
		"./stringlist.c",
	},
	hdrs = hfiles,
	deps = hfiles,
}

