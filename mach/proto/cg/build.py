from build.ab import Rule, simplerule, Targets
from build.c import cprogram
from util.cgg.build import cgg


@Rule
def build_cg(self, name, arch, cflags=[], deps: Targets = []):
    tables = cgg(
        name=f"{self.localname}/tables",
        srcs=[f"mach/{arch}/cg/table"],
        cflags=cflags,
        deps=[f"mach/{arch}/cg"] + deps,
    )

    cprogram(
        replaces=self,
        cflags=cflags,
        srcs=[
            "mach/proto/cg/assert.h",
            "mach/proto/cg/codegen.h",
            "mach/proto/cg/data.h",
            "mach/proto/cg/equiv.h",
            "mach/proto/cg/extern.h",
            "mach/proto/cg/fillem.h",
            "mach/proto/cg/gencode.h",
            "mach/proto/cg/glosym.h",
            "mach/proto/cg/param.h",
            "mach/proto/cg/reg.h",
            "mach/proto/cg/regvar.h",
            "mach/proto/cg/result.h",
            "mach/proto/cg/salloc.h",
            "mach/proto/cg/state.h",
            "mach/proto/cg/subr.h",
            "mach/proto/cg/types.h",
            "mach/proto/cg/codegen.c",
            "mach/proto/cg/compute.c",
            "mach/proto/cg/equiv.c",
            "mach/proto/cg/fillem.c",
            "mach/proto/cg/gencode.c",
            "mach/proto/cg/glosym.c",
            "mach/proto/cg/main.c",
            "mach/proto/cg/move.c",
            "mach/proto/cg/nextem.c",
            "mach/proto/cg/reg.c",
            "mach/proto/cg/regvar.c",
            "mach/proto/cg/salloc.c",
            "mach/proto/cg/state.c",
            "mach/proto/cg/subr.c",
            "mach/proto/cg/var.c",
            tables,
        ],
        deps=(
            [
                "h",
                "modules/h",
                "modules/src/flt_arith",
                "modules/src/object",
                "modules/src/em_data",
                f"mach/{arch}/cg",
            ]
            + deps
        ),
    )
