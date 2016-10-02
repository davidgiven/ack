cprogram {
	name = "cvmach",
	srcs = { "./cvmach.c" },
	deps = {
		"h+emheaders",
		-- Next line only because object.h includes ansi.h
		"modules+headers",
		"modules/src/object+lib",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/cvmach"] = "+cvmach",
	}
}
