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
		"./*.c",
		"./*.h",
		"+tables",
	},
	deps = {
		"h+emheaders",
		"h+local",
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

