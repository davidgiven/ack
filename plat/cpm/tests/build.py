from tests.plat.build import plat_testsuite

plat_testsuite(
    name="tests",
    plat="cpm",
    method="plat/cpm/emu",
    extratests=["./parsefcb,c,.c"],
    sets=["core", "bugs", "m2"],
)
