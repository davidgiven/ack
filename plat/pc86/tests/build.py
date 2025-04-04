from tests.plat.build import plat_testsuite

plat_testsuite(
    name="tests",
    plat="pc86",
    method="plat/pc86/emu",
    sets=["core", "bugs", "m2"],
)
