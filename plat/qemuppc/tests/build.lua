include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "qemuppc",
    method = "qemu-system-ppc"
}
