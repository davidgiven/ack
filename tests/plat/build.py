from build.ab import Rule, Target, Targets, export, filenamesof
from build.ack import ackclibrary, ackcprogram
from glob import glob
from os.path import *

allsets = {
    "core": [],
    "bugs": [
        "tests/plat/bugs/bug-157-i80-varargs,c,.c",
        "tests/plat/bugs/bug-164-faddrn,c,.c",
        "tests/plat/bugs/bug-203-ego-sr,c,-O3,.c",
        "tests/plat/bugs/bug-22-inn,mod,.mod",
        "tests/plat/bugs/bug-62-notvar_var,e,.c",
        "tests/plat/bugs/bug-310-cpp-no-trailing-newline,c,.c",
    ],
    "m2": [],
    "floats": [],
    "long-long": [],
}


@Rule
def plat_testsuite(
    self,
    name,
    plat,
    method: Target,
    sets=["core", "bugs", "m2", "floats", "long-long"],
    extratests: Targets = [],
):
    lib = ackclibrary(
        name="lib",
        srcs=["tests/plat/lib/test.c"],
        hdrs={
            "test.h": "tests/plat/lib/test.h",
            "Test.def": "tests/plat/lib/Test.def",
        },
        deps=[f"lang/cem/libcc.ansi+lib_{plat}"],
        plat=plat,
    )

    testfiles = filenamesof(extratests)
    for set in sets:
        testfiles += allsets[set]

    tests = []
    for test in testfiles:
        filename, lang, *flags = test.split(",")
        flags = flags[0:-1]
        fs = basename(filename)

        tests += [
            ackcprogram(
                name=f"{fs}_bin",
                srcs=[test],
                lang=lang,
                plat=plat,
                cflags=flags,
                deps=[lib],
            )
        ]

    export(replaces=self, deps=tests)
