local installmap = {}

local function build_descr(name)
	local descr = normalrule {
		name = name,
		ins = {
			"lang/cem/cpp.ansi+cpp",
			"./"..name..".descr",
			"./descr.sed",
			matching(filenamesof("modules/src/em_data+lib"), "em_mnem%.h$"),
		},
		outleaves = { name..".descr" },
		commands = {
			"%{ins[1]} -P -I%{dirname(ins[4])} %{ins[2]} > %{dir}/temp",
			"sed -f %{ins[3]} < %{dir}/temp > %{outs}"
		}
	}

	installmap["$(PLATIND)/ego/"..name..".descr"] = descr
end

build_descr("i386")
build_descr("i86")
build_descr("m68020")
build_descr("powerpc")

installable {
	name = "pkg",
	map = installmap
}

