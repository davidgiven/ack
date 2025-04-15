from build.ab import export
from build.pascal import pascalprogram

pascalprogram(
    name="tailor",
    fpcflags=["-Miso"],
    srcs=["./tailor.p"])

export(name="all",
       items={"$(PLATDEP)/tailor": ".+tailor"})
