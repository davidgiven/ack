local installmap = {}

local function build_man(name, sect)
	-- TODO: figure out if the ./head file is really useful.  -- tkchia
	local man = name.."."..sect

	installmap["$(INSDIR)/share/man/man"..sect.."/"..man] = "./"..man
end

build_man("6500_as", "6")
build_man("6800_as", "6")
build_man("6805_as", "6")
build_man("6809_as", "6")
build_man("em_cg", "6")
build_man("em_ncg", "6")
build_man("i386_as", "6")
build_man("i80_as", "6")
build_man("i86_as", "6")
build_man("m68020_as", "6")
build_man("ns_as", "6")
build_man("pdp_as", "6")
build_man("powerpc_as", "6")
build_man("uni_ass", "6")
build_man("vc4_as", "6")
build_man("z8000_as", "6")
build_man("z80_as", "6")

build_man("libmon", "7")
build_man("libpc", "7")
build_man("pc_prlib", "7")

installable {
	name = "pkg",
	map = installmap
}
