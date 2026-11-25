from tests.plat.build import plat_testsuite

plat_testsuite(
    name="tests",
    plat="e6800",
    method="plat/e6800/tests/test-wrapper.sh",
   extratests=[ ],
    sets=["core", "bugs", "m2"],
)
