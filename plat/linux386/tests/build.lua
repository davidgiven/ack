include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "linux386",
    method = "qemu-i386"
}
