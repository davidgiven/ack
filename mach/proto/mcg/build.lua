include("util/mcgg/build.lua")

mcgg {
	name = "mcgg_c",
	srcs = { "./table" }
}

cprogram {
	name = "mcg",
	srcs = {
		"./*.c",
		"+mcgg_c",
	},
	deps = {
		"util/mcgg+lib",
		"h+emheaders",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/data+lib",
		"modules/src/em_code+lib_k",
		"modules/src/em_data+lib",
		"modules/src/idf+lib",
		"modules/src/read_em+lib_ev",
		"modules/src/string+lib",
		"modules/src/system+lib",
		"./*.h",
	},
	vars = {
		["+cflags"] = {
			"-Werror-implicit-function-declaration",
			"-Wint-conversion"
		}
	}
}

-- Just for test purposes for now
installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/mcg"] = "+mcg"
	}
}

