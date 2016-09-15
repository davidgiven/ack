include("plat/build.lua")

tabgen {
	name = "ctype_tab",
	srcs = { "./ctype/char.tab" }
}

normalrule {
	name = "ctype_files",
	ins = { "./ctype/genfiles" },
	outleaves = {
		"isalnum.c",
		"isalpha.c",
		"isascii.c",
		"iscntrl.c",
		"isdigit.c",
		"isgraph.c",
		"islower.c",
		"isprint.c",
		"ispunct.c",
		"isspace.c",
		"isupper.c",
		"isxdigit.c",
	},
	commands = {
		"sh %{ins[1]} %{dir}"
	}
}

for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"+ctype_files",
			"+ctype_tab",
			"./ctype/*.c",
			"./errno/*.c",
			"./locale/*.c",
			"./malloc/*.c",
			"./math/*.c",
			"./math/*.e",
			"./misc/environ.c", -- don't build everything here as it's all obsolete
			"./setjmp/*.c",
			"./setjmp/*.e",
			"./signal/*.c",
            "./assert/*.c",
			"./stdio/*.c",
			"./stdlib/*.c",
			"./string/*.c",
			"./time/*.c",
			
        },
		hdrs = {}, -- must be empty
		deps = {
			"lang/cem/libcc.ansi/headers+headers",
			"plat/"..plat.."/include+headers",
		},
        vars = { plat = plat }
    }

	ackfile {
		name = "crt_"..plat,
		srcs = { "./head_ac.e" },
		vars = { plat = plat },
		deps = {
			"h+emheaders"
		}
	}

	installable {
		name = "pkg_"..plat,
		map = {
			"lang/cem/libcc.ansi/headers+pkg",
			["$(PLATIND)/"..plat.."/c-ansi.o"] = "+crt_"..plat,
			["$(PLATIND)/"..plat.."/libc.a"] = "+lib_"..plat,
		}
	}
end

