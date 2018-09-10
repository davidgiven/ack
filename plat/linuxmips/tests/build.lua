include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "linuxmips",
    method = "qemu-mipsel",
    skipsets = {"m2"}
}
