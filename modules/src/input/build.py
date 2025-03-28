from build.c import clibrary
from glob import glob


clibrary(
    name="input",
    srcs=["./AtEoIF.c", "./AtEoIT.c"],
    hdrs={"inp_pkg.body": "./inp_pkg.body", "inp_pkg.spec": "./inp_pkg.spec"},
)
