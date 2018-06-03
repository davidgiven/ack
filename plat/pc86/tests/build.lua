include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "pc86",
    method = "plat/pc86/emu+8086tiny",
}
