from build.ab import Rule, Targets, simplerule
from build.utils import collectattrs
from build.c import cprogram, clibrary, cppfile
from build.yacc import bison


@Rule
def build_as(self, name, arch, deps: Targets = []):
    archlib = clibrary(
        name=f"{self.localname}/arch",
        hdrs={
            k: f"mach/{arch}/as/{k}"
            for k in [
                "mach0.c",
                "mach1.c",
                "mach2.c",
                "mach3.c",
                "mach4.c",
                "mach5.c",
            ]
        },
        deps=deps,
    )

    preprocessedy = cppfile(
        name=f"{self.localname}/bisoninput",
        srcs=["mach/proto/as/comm2.y"],
        deps=(
            ["mach/proto/as/comm0.h", "mach/proto/as/comm1.h", "h", archlib]
            + deps
        ),
    )

    bisonfiles = bison(name=f"{self.localname}/bison", src=preprocessedy)

    cprogram(
        replaces=self,
        srcs=[
            "mach/proto/as/comm0.h",
            "mach/proto/as/comm1.h",
            "mach/proto/as/comm3.c",
            "mach/proto/as/comm4.c",
            "mach/proto/as/comm5.c",
            "mach/proto/as/comm6.c",
            "mach/proto/as/comm7.c",
            "mach/proto/as/comm8.c",
            bisonfiles,
        ],
        deps=(
            ["h", "modules/src/object", "modules/src/flt_arith", archlib] + deps
        ),
    )
