cprogram {
	name = "makecldef",
	srcs = { "./makecldef.c" }
}

normalrule {
	name = "classdefs_h",
	ins = {
		"+makecldef",
		matching(filenamesof("modules/src/em_data+lib"), "em_mnem%.h$"),
		"./cldefs.src"
	},
	outleaves = { "classdefs.h" },
	commands = {
		"%{ins} > %{outs}"
	}
}

normalrule {
	name = "pop_push_h",
	ins = {
		"./pop_push.awk",
		"h/em_table",
	},
	outleaves = { "pop_push.h" },
	commands = {
		"awk -f %{ins[1]} < %{ins[2]} > %{outs}"
	}
}

clibrary {
	name = "lib",
	srcs = {
		"./debug.c",
		"./global.c",
		"./files.c",
		"./go.c",
		"./map.c",
		"./utils.c",
		"./get.c",
		"./put.c",
		"./alloc.c",
		"./lset.c",
		"./cset.c",
		"./parser.c",
		"./stack_chg.c",
		"./locals.c",
		"./init_glob.c",
	},
	deps = {
		"./*.h",
		"+classdefs_h",
		"+pop_push_h",
		"h+emheaders",
		"modules/src/em_data+lib",
	},
	vars = {
		["+cflags"] = {"-DVERBOSE", "-DNOTCOMPACT"}
	}
}
