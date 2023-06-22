include("tests/plat/build.lua")

plat_testsuite {
    name = "tests",
    plat = "agonmos",
    method = "plat/agonmos/emu+emu",
    skipsets = {
        "b",      -- B doesn't work on the ez80
        "floats", -- floats aren't supported
        "long-long",
		"m2",
		"pc",
		"core",
    },
}
