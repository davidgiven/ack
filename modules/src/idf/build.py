from build.c import clibrary
from glob import glob


clibrary(
    name="idf",
    hdrs={"idf_pkg.body": "./idf_pkg.body", "idf_pkg.spec": "./idf_pkg.spec"},
)
