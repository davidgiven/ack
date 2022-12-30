normalrule {
	name = "c_mnem_narg_h",
	ins = {
		"./make_C_mnem_narg_h.lua",
		"h/em_table_lib.lua",
		"h/em_table",
	},
	outleaves = "C_mnem_narg.h",
	commands = {
		"$(LUA) %{ins[1]} < %{ins[3]} > %{outs}"
	}
}
	
normalrule {
	name = "c_mnem_h",
	ins = {
		"./make_C_mnem_h.lua",
		"h/em_table_lib.lua",
		"h/em_table",
	},
	outleaves = "C_mnem.h",
	commands = {
		"$(LUA) %{ins[1]} < %{ins[3]} > %{outs}"
	}
}
	
local function variant(name, cflags)
	clibrary {
		name = name,
		vars = {
			["+cflags"] = {
				"-DPRIVATE=static",
				"-DEXPORT=",
				"-DNDEBUG",
				"-DCHECKING",
				unpack(cflags)
			},
		},
		srcs = {
			"./EM_vars.c",
			"./read_em.c",
			"./mkcalls.c",
		},
		hdrs = {
			"./em_comp.h",
		},
		deps = {
			"+c_mnem_h",
			"+c_mnem_narg_h",
			"h+emheaders",
			"modules+headers",
			"modules/src/alloc+lib",
			"modules/src/em_code+headers",
			"modules/src/em_data+lib",
			"modules/src/string+lib",
			"modules/src/system+lib",
			"./em_comp.h",
		}
	}
end

variant("lib_ev", {})
variant("lib_kv", { "-DCOMPACT" })
