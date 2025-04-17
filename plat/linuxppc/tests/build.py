from tests.plat.build import plat_testsuite

plat_testsuite(
    name="tests",
    plat="linuxppc",
    method="plat/linuxppc/emu",
    sets=["core", "bugs", "m2", "floats"],
)
