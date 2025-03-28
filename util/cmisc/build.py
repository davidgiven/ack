from build.ab import Rule, Target, simplerule
from build.c import hostcprogram

hostcprogram(name="tabgen", srcs=["./tabgen.c"])


@Rule
def tabgen(self, name, src: Target):
    simplerule(
        replaces=self,
        ins=["util/cmisc+tabgen", src],
        outs=[f"={self.localname}.c"],
        commands=["$[ins[0]] -f$[ins[1]] > $[outs]"],
        label="TABGEN",
    )
