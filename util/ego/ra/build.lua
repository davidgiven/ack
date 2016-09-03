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
	srcs = { "./ra*.c" },
	deps = {
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
		"+itemtab_h",
	},
	vars = {
		["+cflags"] = {"-DVERBOSE", "-DNOTCOMPACT"}
	}
}

