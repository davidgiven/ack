cprogram {
	name = "maktab",
	srcs = {
		"./maktab.c"
	},
	deps = {
		"h+emheaders",
		"modules/src/em_data+lib",
	}
}

normalrule {
	name = "asstb",
	ins = {
		"+maktab",
		"etc/ip_spec.t"
	},
	outleaves = {
		"asstb.c"
	},
	commands = {
		"%{ins} %{outs}"
	}
}

cprogram {
	name = "em_ass",
	srcs = {
		"./ass*.c",
		"+asstb",
	},
	deps = {
		"h+emheaders",
		"h+local",
		--"modules/src/alloc+lib",
		"modules/src/em_data+lib",
		--"modules/src/data+lib",
		--"modules/src/object+lib",
		--"modules/src/system+lib",
		"./ass*.h",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_ass"] = "+em_ass",
		["$(INSDIR)/share/man/man6/em_ass.6"] = "./em_ass.6",
	}
}

