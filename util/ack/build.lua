cprogram {
	name = "mktables",	
	srcs = { "./mktables.c" },
}

normalrule {
	name = "tables",
	outleaves = { "dmach.c", "intable.c" },
	ins = {
		"+mktables",
		"lib/descr/fe",
	},
	commands = {
		"(cd %{dir} && %{abspath(ins[1])} lib)"
	}
}

cprogram {
	name = "ack",
	srcs = {
		"./data.c",
		"./files.c",
		"./grows.c",
		"./list.c",
		"./main.c",
		"./rmach.c",
		"./run.c",
		"./scan.c",
		"./svars.c",
		"./trans.c",
		"./util.c",
		"+tables",
	},
	deps = {
		"h+emheaders",
		"h+local",
		"./ack.h",
		"./data.h",
		"./dmach.h",
		"./grows.h",
		"./list.h",
		"./trans.h",
	}
}

installable {
	name = "pkg",
	map = {
		["$(INSDIR)/bin/ack"] = "+ack",
		["$(INSDIR)/share/man/man1/ack.1"] = "./ack.1.X",
		["$(PLATIND)/descr/fe"] = "lib/descr/fe",
	}
}

