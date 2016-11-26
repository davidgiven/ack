include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "pc86",
    method = "qemu-system-i386"
}
