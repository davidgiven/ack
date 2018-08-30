include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "pc86",
    method = "plat/pc86/emu+pc86emu",
	skipsets = {"floats"}, -- FPU instructions not supported by emulator
}
