from tests.plat.build import plat_testsuite

plat_testsuite(
    name="tests",
    plat="linux68k",
    method="plat/linux68k/emu",
    sets=["core", "bugs", "m2"],
)
