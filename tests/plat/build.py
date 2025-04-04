from build.ab import Rule, Target, Targets, export, filenamesof
from build.ack import ackclibrary, ackcprogram
from build.utils import test
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
        "tests/plat/core/aar,e,.e",
        "tests/plat/core/and,e,.e",
        "tests/plat/core/andv,e,.e",
        "tests/plat/core/brk,c,.c",
        "tests/plat/core/bss,e,.c",
        "tests/plat/core/calloc,c,.c",
        "tests/plat/core/cii,e,.e",
        "tests/plat/core/cmi,e,.e",
        "tests/plat/core/cms,e,.e",
        "tests/plat/core/cmu,e,.e",
        "tests/plat/core/csa,e,.c",
        "tests/plat/core/csb,e,.c",
        "tests/plat/core/_dummy,e,.c",
        "tests/plat/core/dup,e,.e",
        "tests/plat/core/exg,e,.e",
        "tests/plat/core/inn,e,.e",
        "tests/plat/core/intadd,e,.c",
        "tests/plat/core/intcmp,e,.c",
        "tests/plat/core/intdiv,e,.c",
        "tests/plat/core/intrem,e,.c",
        "tests/plat/core/intshift,e,.c",
        "tests/plat/core/intsub,e,.c",
        "tests/plat/core/ior,e,.e",
        "tests/plat/core/iorv,e,.e",
        "tests/plat/core/lar,e,.e",
        "tests/plat/core/newdispose,p,.p",
        "tests/plat/core/pascalsets,p,.p",
        "tests/plat/core/rck,e,.e",
        "tests/plat/core/rotate,e,.e",
        "tests/plat/core/sar,e,.e",
        "tests/plat/core/set,e,.e",
        "tests/plat/core/setjmp,c,.c",
        "tests/plat/core/structcopy,e,.c",
        "tests/plat/core/xor,e,.e",
        "tests/plat/core/xorv,e,.e",
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
    for t in testfiles:
        filename, lang, *flags = t.split(",")
        flags = flags[0:-1]
        fs = basename(filename)

        tests += [
            test(
                name=f"{fs}_test",
                ins=[
                    "tests/plat/testdriver.sh",
                    method,
                    "util/build+testrunner",
                    ackcprogram(
                        name=f"{fs}_bin",
                        srcs=[t],
                        lang=lang,
                        plat=plat,
                        cflags=flags,
                        deps=[lib],
                    ),
                ],
                commands=["$[ins[0]] $[ins[1]] $[ins[3]] 15 $[ins[2]]"],
            )
        ]

    export(replaces=self, deps=tests)
