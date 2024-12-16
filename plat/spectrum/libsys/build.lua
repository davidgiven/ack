local generated = {}

acklibrary {
    name = "internal",
    hdrs = {
		"./asm.h",
	}
}

local trap_calls = {
    "EARRAY",
    "EBADGTO",
    "EBADLAE",
    "EBADLIN",
    "EBADMON",
    "EBADPC",
    "EBADPTR",
    "ECASE",
    "ECONV",
    "EFDIVZ",
    "EFOVFL",
    "EFUND",
    "EFUNFL",
    "EHEAP",
    "EIDIVZ",
    "EILLINS",
    "EIOVFL",
    "EIUND",
    "EMEMFLT",
    "EODDZ",
    "ERANGE",
    "ESET",
    "ESTACK",
    "EUNIMPL",
	"EMON",
}

for _, name in pairs(trap_calls) do
    generated[#generated+1] = normalrule {
        name = name,
        ins = { "./make_trap.sh" },
        outleaves = { name..".s" },
        commands = {
            "%{ins[1]} "..name:lower().." "..name.." > %{outs}"
        }
    }
end

acklibrary {
    name = "lib",
    srcs = {
		"./brk.c",
		"./close.c",
		"./creat.c",
		"./errno.s",
		"./getpid.c",
		"./_hol0.s",
		"./isatty.c",
		"./kill.c",
		"./lseek.c",
		"./open.c",
		"./read.c",
		"./signal.c",
		"./_sys_rawread.s",
		"./_sys_rawwrite.s",
		"./_trap.s",
		"./time.c",
		"./write.c",
		generated
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/spectrum/include+headers",
		"+internal",
	},
    vars = {
        plat = "spectrum"
    }
}

