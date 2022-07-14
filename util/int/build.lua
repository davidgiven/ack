normalrule {
	name = "trap_msg",
	ins = {
		"./M.trap_msg",
		"etc/traps"
	},
	outleaves = {"trap_msg"},
	commands = {
		"%{ins} > %{outs}"
	}
}

normalrule {
	name = "warn_msg",
	ins = {
		"./M.warn_msg",
		"etc/traps"
	},
	outleaves = {"warn_msg"},
	commands = {
		"%{ins} > %{outs}"
	}
}

normalrule {
	name = "warn_h",
	ins = {
		"./M.warn_h",
		"doc/int/appA" -- there's a 1991 comment saying this needs to be moved
	},
	outleaves = {"warn.h"},
	commands = {
		"%{ins} > %{outs}"
	}
}

cprogram {
	name = "mkiswitch",
	srcs = {"./switch/mkiswitch.c"},
	deps = {
		"h+emheaders",
		"modules/src/em_data+lib",
	}
}

normalrule {
	name = "docases",
	ins = {
		"+mkiswitch",
		"etc/ip_spec.t"
	},
	outleaves = {"DoCases"},
	commands = {
		"%{ins[1]} Do %{ins[2]} %{outs}"
	}
}

cprogram {
	name = "mkswitch",
	srcs = {"./switch/mkswitch.c"}
}

normalrule {
	name = "prcases",
	ins = {
		"+mkswitch",
		"etc/ip_spec.t"
	},
	outleaves = {"PrCases"},
	commands = {
		"%{ins[1]} Pr %{ins[2]} %{outs}"
	}
}

clibrary {
	name = "generated",
	srcs = {},
	hdrs = {
		"+warn_msg",
		"+trap_msg",
		"+warn_h",
		"+docases",
		"+prcases",
	}
}

cprogram {
	name = "int",
	srcs = {
		"./alloc.c",
		"./core.c",
		"./data.c",
		"./disassemble.c",
		"./do_array.c",
		"./do_branch.c",
		"./do_comp.c",
		"./do_conv.c",
		"./do_fpar.c",
		"./do_incdec.c",
		"./do_intar.c",
		"./do_load.c",
		"./do_logic.c",
		"./do_misc.c",
		"./do_proc.c",
		"./do_ptrar.c",
		"./do_sets.c",
		"./do_store.c",
		"./do_unsar.c",
		"./dump.c",
		"./fra.c",
		"./global.c",
		"./init.c",
		"./io.c",
		"./log.c",
		"./main.c",
		"./m_ioctl.c",
		"./moncalls.c",
		"./monstruct.c",
		"./m_sigtrp.c",
		"./proctab.c",
		"./read.c",
		"./rsb.c",
		"./segment.c",
		"./stack.c",
		"./switch.c",
		"./tally.c",
		"./text.c",
		"./trap.c",
		"./warn.c",
	},
	deps = {
		"h+emheaders",
		"+generated",
		"modules/src/em_data+lib",
	},
	vars = {
		["+cflags"] = {"-Wno-implicit-int"}
	}
}

installable {
	name = "pkg",
	map = {
		["$(INSDIR)/bin/int"] = "+int",
		["$(INSDIR)/share/man/man1/int.1"] = "./int.1",
	}
}


