from build.ab import export, Rule, Targets, simplerule, filenamesof
from build.pascal import pascalprogram

pascalprogram(name="tailor", fpcflags=["-Miso"], srcs=["./tailor.p"])

@Rule
def tailor(self, name, srcs: Targets = [], numbers=[]):
    nlist = " ".join([str(n) for n in numbers])
    simplerule(
        replaces=self,
        ins=srcs,
        deps=["lang/a68s/util+tailor"],
        outs=[f"={self.localname}"],
        label="TAILOR",
        commands=[
            f"echo {nlist} | $[deps[0]] {ff} >> $[outs[0]]"
            for ff in filenamesof(srcs)
        ],
    )
