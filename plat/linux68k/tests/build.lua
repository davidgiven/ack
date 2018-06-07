include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "linux68k",
    method = "plat/linux68k/emu+emu68k",
}
