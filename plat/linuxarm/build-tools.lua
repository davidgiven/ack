include("plat/build.lua")
include("lang/pcc/build.lua")

build_as {
	name = "as",
	arch = "arm",
}

build_pcc {
	name = "pcc_ccom",
	pccarch = "arm",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/linuxarm/as"] = "+as",
		["$(PLATDEP)/linuxarm/pcc_ccom"] = "+pcc_ccom",
		["$(PLATIND)/descr/linuxarm"] = "./descr",
	}
}
