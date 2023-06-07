acklibrary {
    name = "internal",
    hdrs = {
		"./asm.h"
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
}

local generated = {}
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
        generated
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
        "plat/cpm/include+headers",
        "+internal",
	},
    vars = {
        plat = "agonmos"
    }
}

