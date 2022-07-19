normalrule {
	name = "em_path",
	ins = {},
	outleaves = { "em_path.h" },
	commands = {
		"echo '#define EM_DIR \"$(PREFIX)\"' > %{outs}",
		"echo '#define ACK_PATH \"share/ack/descr\"' >> %{outs}",
	}
}

normalrule {
	name = "local",
	ins = {},
	outleaves = { "local.h" },
	commands = {
		"echo '#define VERSION 3' > %{outs}",
		"echo '#define ACKM \"$(DEFAULT_PLATFORM)\"' >> %{outs}",
		"echo '#define BIGMACHINE 1' >> %{outs}",
		"echo '#define SYS_5' >> %{outs}",
	}
}

clibrary {
	name = "emheaders",
	hdrs = {
		"./arch.h",
		"./as_spec.h",
		"./cgg_cg.h",
		"./cg_pattern.h",
		"./em_abs.h",
		"./em_ego.h",
		"./em_flag.h",
		"./em_mes.h",
		"./em_ptyp.h",
		"./em_reg.h",
		"./ip_spec.h",
		"./ocm_chan.h",
		"./ocm_parco.h",
		"./ocm_proc.h",
		"./out.h",
		"./ranlib.h",
		"./stb.h",
		"./con_float",
		"+em_path",
		"+local",
	}
}

