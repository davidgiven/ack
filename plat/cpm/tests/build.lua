include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "cpm",
    method = "plat/cpm/emu+emu",
    skipsets = {
        "b",      -- B is broken on i80
        "floats", -- floats aren't supported
        "long-long",
    },
    tests = { "./*.c" },
}
