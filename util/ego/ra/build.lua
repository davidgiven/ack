cprogram {
	name = "makeitems",
	srcs = { "./makeitems.c" }
}

normalrule {
	name = "itemtab_h",
	ins = {
		"+makeitems",
		matching(filenamesof("modules/src/em_data+lib"), "em_mnem%.h$"),
		"./itemtab.src"
	},
	outleaves = { "itemtab.h" },
	commands = {
		"%{ins} > %{outs}"
	}
}

cprogram {
	name = "ra",
	srcs = {
		"./ra_allocl.c",
		"./ra_aux.c",
		"./ra.c",
		"./ra_interv.c",
		"./ra_items.c",
		"./ra_lifet.c",
		"./ra_pack.c",
		"./ra_profits.c",
		"./ra_xform.c",
	},
	deps = {
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
		"+itemtab_h",
		"./ra_allocl.h",
		"./ra_aux.h",
		"./ra.h",
		"./ra_interv.h",
		"./ra_items.h",
		"./ra_lifet.h",
		"./ra_pack.h",
		"./ra_profits.h",
		"./ra_xform.h",
	},
	vars = {
		["+cflags"] = {"-DVERBOSE", "-DNOTCOMPACT"}
	}
}

