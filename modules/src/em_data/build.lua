local GENFILES = {
	"flag.c",
	"mnem.c",
	"mnem.h",
	"pseu.c",
	"pseu.h",
	"spec.h"
}

local generated = {}
for _, f in ipairs(GENFILES) do
	generated[#generated+1] = normalrule {
		name = "em_"..f:gsub("%.", "_"),
		ins = {
			"./make_"..f:gsub("%.", "_")..".lua",
			"h/em_table_lib.lua",
			"h/em_table", -- relative to root, which is a bit evil
		},
		outleaves = {
			"em_"..f
		},
		deps = {
			"h+emheaders"
		},
		commands = {
			"$(LUA) %{ins[1]} < %{ins[3]} > %{outs}"
		}
	}
end

clibrary {
	name = "lib",
	srcs = concat(
		"./em_ptyp.c",
		matching(filenamesof(generated), "%.c$")
	),
	hdrs = {
		generated, -- so we export the H files
	},
	deps = {
		generated, -- so we can see the H files
		"h+emheaders"
	}
}
