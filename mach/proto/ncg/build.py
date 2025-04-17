from build.ab import Rule, simplerule, Targets
from build.c import cprogram
from util.ncgg.build import ncgg


@Rule
def build_ncg(self, name, arch, cflags=[], deps: Targets = []):
    tables = ncgg(
        name=f"{self.localname}/tables",
        srcs=[f"mach/{arch}/ncg/table"],
        cflags=cflags,
        deps=[f"mach/{arch}/ncg"] + deps,
    )

    cprogram(
        replaces=self,
        cflags=cflags,
        srcs=[
            "mach/proto/ncg/codegen.c",
            "mach/proto/ncg/compute.c",
            "mach/proto/ncg/data.h",
            "mach/proto/ncg/equiv.c",
            "mach/proto/ncg/equiv.h",
            "mach/proto/ncg/extern.h",
            "mach/proto/ncg/fillem.c",
            "mach/proto/ncg/gencode.c",
            "mach/proto/ncg/glosym.c",
            "mach/proto/ncg/glosym.h",
            "mach/proto/ncg/label.c",
            "mach/proto/ncg/label.h",
            "mach/proto/ncg/main.c",
            "mach/proto/ncg/move.c",
            "mach/proto/ncg/nextem.c",
            "mach/proto/ncg/param.h",
            "mach/proto/ncg/reg.c",
            "mach/proto/ncg/regvar.c",
            "mach/proto/ncg/regvar.h",
            "mach/proto/ncg/result.h",
            "mach/proto/ncg/salloc.c",
            "mach/proto/ncg/state.c",
            "mach/proto/ncg/state.h",
            "mach/proto/ncg/subr.c",
            "mach/proto/ncg/types.h",
            "mach/proto/ncg/var.c",
            "mach/proto/ncg/xmach.h",
            tables,
        ],
        deps=(
            [
                "h",
                "modules/h",
                "modules/src/flt_arith",
                "modules/src/object",
                "modules/src/em_data",
                f"mach/{arch}/ncg",
            ]
            + deps
        ),
    )
