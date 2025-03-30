from build.c import cprogram

cprogram(
    name="cv",
    srcs=["./cv.c"],
    deps=["h", "modules/src/data", "modules/src/object"],
)
