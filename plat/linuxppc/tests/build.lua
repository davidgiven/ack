include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "linuxppc",
    method = "plat/linuxppc/emu+emuppc"
}
