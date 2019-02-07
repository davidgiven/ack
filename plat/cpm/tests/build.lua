include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "cpm",
    method = "plat/cpm/emu+emu"
}
