from build.ab import Rule, Target
from build.c import cprogram
from util.topgen.build import topgen


@Rule
def build_top(self, name, arch):
    t = topgen(name=f"{self.localname}_topgen", src=f"mach/{arch}/top/table")

    cprogram(
        replaces=self,
        srcs=[
            "mach/proto/top/queue.c",
            "mach/proto/top/queue.h",
            "mach/proto/top/top.c",
            "mach/proto/top/top.h",
            t,
        ],
    )
