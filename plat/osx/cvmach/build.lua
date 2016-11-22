cprogram {
	name = "cvmach",
	srcs = { "./cvmach.c" },
	deps = {
		"h+emheaders",
		"modules/src/object+lib",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/cvmach"] = "+cvmach",
	}
}
