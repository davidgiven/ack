cprogram {
	name = "emu",
	srcs = {
		"./main.c",
		"./emulator.c",
		"./cemu/core/cpu.c",
		"./cemu/core/registers.c",
		"./zdis/zdis.c",
	},
	vars = {
		["+ldflags"] = {"-lreadline"}
	},
}

