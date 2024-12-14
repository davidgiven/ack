local hh_files = {
    "./tree.hh",
    "./file.hh",
    "./symbol.hh",
    "./type.hh",
}

local cc_files = {
    "./avl.cc",
    "./itemlist.cc",
    "./langdep.cc",
    "./scope.cc",
}

local hh_bases = basename(filenamesof(hh_files))
local cc_bases = basename(filenamesof(cc_files))

local h_targets = {}
for _, f in ipairs(hh_bases) do
    local bf = f:gsub("%..*$", ""):gsub("^$./", "")
    h_targets[#h_targets+1] = normalrule {
        name = "hh_header/"..bf,
        ins = { "./make.allocd", "./"..f },
        outleaves = { bf..".h" },
        commands = {
                "%{ins[1]} < %{ins[2]} > %{outs}"
        }
    }
end

local c_targets = {}
for _, f in ipairs(cc_bases) do
    local bf = f:gsub("%..*$", ""):gsub("^$./", "")
    c_targets[#h_targets+1] = normalrule {
        name = "cc_header/"..bf,
        ins = { "./make.allocd", "./"..f },
        outleaves = { bf..".c" },
        commands = {
                "%{ins[1]} < %{ins[2]} > %{outs}"
        }
    }
end

normalrule {
    name = "tokenfile_g",
    ins = { "./make.tokfile", "./tokenname.c" },
    outleaves = { "tokenfile.g" },
    commands = {
        "%{ins[1]} < %{ins[2]} > %{outs}"
    }
}

normalrule {
    name = "tokenname_c",
    ins = { "./make.tokcase", "./tokenname.c" },
    outleaves = { "symbol2str.c" },
    commands = {
        "%{ins[1]} < %{ins[2]} > %{outs}"
    }
}

normalrule {
    name = "ops",
    ins = { "./make.ops", "./operators.ot" },
    outleaves = { "ops.c", "ops.h" },
    commands = {
        "%{ins[1]} %{ins[2]} %{outs[1]} %{outs[2]}"
    }
}

llgen {
    name = "commands_llgen",
    srcs = {
        -- order here is important
        "+tokenfile_g",
        "./commands.g",
    }
}

llgen {
    name = "db_symtab_llgen",
    basename = "DBSpars",
    srcs = {
        -- order here is important
        "./db_symtab.g",
    }
}


cprogram {
	name = "grind",
	srcs = {
		"./c.c",
        "./do_comm.c",
        "./expr.c",
        "./idf.c",
        "./list.c",
        "./main.c",
        "./modula-2.c",
        "./pascal.c",
        "./position.c",
        "./print.c",
        "./run.c",
        "./symbol.c",
        "./tokenname.c",
        "./tree.c",
        "./type.c",
        "+tokenname_c",
        matching(filenamesof("+commands_llgen"), "%.c$"),
        matching(filenamesof("+db_symtab_llgen"), "%.c$"),
        matching(filenamesof("+ops"), "%.c$"),
        c_targets,
	},
	deps = {
		"modules/src/data+lib",
		"modules/src/string+lib",
		"modules/src/system+lib",
        "+commands_llgen",
        "+db_symtab_llgen",
        "+ops",
        "./class.h",
        "./expr.h",
        "./idf.h",
        "./langdep.h",
        "./message.h",
        "./misc.h",
        "./operator.h",
        "./position.h",
        "./rd.h",
        "./scope.h",
        "./token.h",
        "./tokenname.h",
        "./print.h",
        "./itemlist.h",
        "h+emheaders",
        "modules+headers",
        "modules/src/alloc+lib",
        "modules/src/idf+lib",
		"modules/src/em_data+lib",
        h_targets,
	}
}

installable {
	name = "pkg",
	map = {
		["$(INSDIR)/bin/grind"] = "+grind",
		["$(INSDIR)/share/man/man1/grind.1"] = "./grind.1",
	}
}


