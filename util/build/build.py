from build.c import hostcprogram

hostcprogram(
    name="testrunner", srcs=["./testrunner.c"], deps=["modules/src/data"]
)
