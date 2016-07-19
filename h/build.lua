normalrule {
	name = "em_path",
	ins = {},
	outleaves = { "em_path.h" },
	commands = {
		"echo '#define TMP_DIR \"$(ACK_TEMP_DIR)\"' > %{outs}",
		"echo '#define EM_DIR \"$(PREFIX)\"' >> %{outs}",
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
		"./*.h",
		"+em_path",
		"+local",
	}
}

