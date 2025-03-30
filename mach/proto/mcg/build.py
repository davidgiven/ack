from build.ab import Rule, Targets
from build.c import cprogram
from util.mcgg.build import mcgg


@Rule
def build_mcg(self, name, arch, cflags=[], deps: Targets = []):
    tables = mcgg(
        name=f"{self.localname}/tables",
        srcs=[f"mach/{arch}/mcg/table"],
        cflags=cflags,
        deps=deps,
    )

    cprogram(
        replaces=self,
        cflags=cflags,
        srcs=[
            "mach/proto/mcg/basicblock.c",
            "mach/proto/mcg/data.c",
            "mach/proto/mcg/graph.c",
            "mach/proto/mcg/hop.c",
            "mach/proto/mcg/ir.c",
            "mach/proto/mcg/main.c",
            "mach/proto/mcg/parse_em.c",
            "mach/proto/mcg/pass_convertstackops.c",
            "mach/proto/mcg/pass_eliminatetrivialblocks.c",
            "mach/proto/mcg/pass_groupirs.c",
            "mach/proto/mcg/pass_instructionselection.c",
            "mach/proto/mcg/pass_livevreganalysis.c",
            "mach/proto/mcg/pass_lowerpushes.c",
            "mach/proto/mcg/pass_phigroups.c",
            "mach/proto/mcg/pass_prologueepilogue.c",
            "mach/proto/mcg/pass_registerallocator.c",
            "mach/proto/mcg/pass_removedeadblocks.c",
            "mach/proto/mcg/pass_removedeadphis.c",
            "mach/proto/mcg/pass_returnvalues.c",
            "mach/proto/mcg/pass_splitcriticaledges.c",
            "mach/proto/mcg/pass_ssa.c",
            "mach/proto/mcg/pass_typeinference.c",
            "mach/proto/mcg/predicates.c",
            "mach/proto/mcg/procedure.c",
            "mach/proto/mcg/reg.c",
            "mach/proto/mcg/symbol.c",
            "mach/proto/mcg/treebuilder.c",
            "mach/proto/mcg/basicblock.h",
            "mach/proto/mcg/graph.h",
            "mach/proto/mcg/hop.h",
            "mach/proto/mcg/ir.h",
            "mach/proto/mcg/mcgg_generated_footer.h",
            "mach/proto/mcg/mcgg_generated_header.h",
            "mach/proto/mcg/mcg.h",
            "mach/proto/mcg/procedure.h",
            "mach/proto/mcg/reg.h",
            f"mach/{arch}/mcg/platform.c",
            tables,
        ],
        deps=(
            [
                "h",
                "modules/h",
                "modules/src/flt_arith",
                "modules/src/object",
                "modules/src/em_data",
                "modules/src/system",
                "modules/src/em_code+lib_k",
                "modules/src/read_em+lib_kv",
                "modules/src/idf",
                "util/mcgg+lib",
            ]
            + deps
        ),
    )
