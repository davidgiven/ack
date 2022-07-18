clibrary {
	name = "lib",
	srcs = {
		"./rd_arhdr.c",
		"./rd_bytes.c",
		"./rd.c",
		"./rd_int2.c",
		"./rd_int4.c",
		"./rd_ranlib.c",
		"./rd_unsig2.c",
		"./wr_arhdr.c",
		"./wr_bytes.c",
		"./wr.c",
		"./wr_int2.c",
		"./wr_int4.c",
		"./wr_putc.c",
		"./wr_ranlib.c",
	},
	hdrs = { "./object.h" },
	deps = {
		"modules+headers",
		"h+local",
		"h+emheaders",
		"./object.h",
		"./obj.h",
	},
}


