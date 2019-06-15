include("plat/build.lua")

tabgen {
	name = "ctype_tab",
	srcs = { "./core/ctype/char.tab" }
}

normalrule {
	name = "ctype_files",
	ins = { "./core/ctype/genfiles" },
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
			"./core/ctype/*.c",
			"./core/errno/*.c",
			"./core/locale/*.c",
			"./core/math/*.c",
			"./core/math/*.e",
			"./core/misc/*.c",
			"./core/printf/*.c",
			"./core/scanf/*.c",
			"./core/setjmp/*.c",
			"./core/setjmp/*.e",
			"./core/stdlib/*.c",
			"./core/stdio/*.c",
			"./core/string/*.c",
			"./core/time/*.c",
			"./sys/exit/*.c",
			"./sys/malloc/*.c",
			"./sys/misc/*.c",
			"./sys/stdio/*.c",
		},
		hdrs = {}, -- must be empty
		deps = {
			"lang/cem/libcc.ansi/headers+pkg",
			"plat/"..plat.."/include+pkg",
			"./core/math/localmath.h",
			"./core/stdlib/ext_fmt.h",
			"./core/time/loc_time.h",
			"./sys/malloc/malloc.h",
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

	local suffix = plat:find("^em") and "m" or "o"
	installable {
		name = "pkg_"..plat,
		map = {
			"lang/cem/libcc.ansi/headers+pkg",
			["$(PLATIND)/"..plat.."/c-ansi."..suffix] = "+crt_"..plat,
			["$(PLATIND)/"..plat.."/libc.a"] = "+lib_"..plat,
		}
	}
end

