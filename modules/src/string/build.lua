clibrary {
	name = "lib",
	srcs = {
		"./bts2str.c", "./btscat.c", "./btscmp.c",
		"./btscpy.c", "./btszero.c", "./long2str.c",
		"./str2bts.c", "./str2long.c", "./strindex.c",
		"./strrindex.c", "./strzero.c",
	},
	deps = {
		"./ack_string.h",
		"modules+headers",
	},
}
