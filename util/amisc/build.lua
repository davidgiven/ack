cprogram {
	name = "aelflod",
	srcs = { "./aelflod.c" },
	deps = {
		"h+emheaders"
	}
}

installable {
	name = "aelflod-pkg",
	map = {
		["$(INSDIR)/bin/aelflod"] = "+aelflod",
		["$(PLATIND)/man/man1/aelflod.1"] = "./aelflod.1",
	}
}

