from build.c import clibrary
from glob import glob


clibrary(
    name="em_mes",
    srcs=(glob("modules/src/em_mes/*.c")),
    deps=["modules/h", "modules/src/em_code+lib_k"],
)
