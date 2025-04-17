from build.ab import export, simplerule, filenamesof, targetof, filenameof
from build.c import cprogram
from glob import glob
from fnmatch import filter

em_mnem = filter(filenamesof([targetof("modules/src/em_data")]), "*/em_mnem.h")[
    0
]
exports = {}


def build_ego_helper(name, srcs=[], deps=[]):
    cprogram(
        name=name,
        srcs=(
            glob(f"util/ego/{name}/*.c") + glob(f"util/ego/{name}/*.h") + srcs
        ),
        deps=[
            "util/ego/share",
            "modules/src/em_data",
            "modules/src/system",
            "h",
        ]
        + deps,
        cflags=["-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"],
    )
    exports[f"$(PLATDEP)/ego/{name}$(EXT)"] = f".+{name}"


def build_descr(name):
    simplerule(
        name=f"{name}_descr",
        ins=[
            "lang/cem/cpp.ansi",
            f"./descr/{name}.descr",
            "./descr/descr.sed",
            em_mnem,
        ],
        outs=[f"={name}.descr"],
        commands=[
            "$[ins[0]] -P -I$[dirname(filenameof(ins[3]))] $[ins[1]] > $[dir]/temp",
            "sed -f $[ins[2]] < $[dir]/temp > $[outs]",
        ],
    )

    exports[f"$(PLATIND)/ego/{name}.descr"] = f".+{name}_descr"


build_ego_helper("bo")
build_ego_helper("ca")
build_ego_helper("cf")
build_ego_helper("cj")
build_ego_helper("cs")
build_ego_helper("ic")
build_ego_helper("il")
build_ego_helper("lv")
build_ego_helper("sp")
build_ego_helper("sr")
build_ego_helper("ud")

build_descr("i386")
build_descr("i86")
build_descr("m68020")
build_descr("powerpc")
build_descr("em22")

cprogram(
    name="em_ego",
    srcs=["./em_ego/em_ego.c"],
    deps=[
        "modules/src/string",
        "modules/src/system",
        "modules/src/data",
        "modules/h",
        "h",
    ],
)

export(
    name="all",
    items={"$(PLATDEP)/em_ego$(EXT)": ".+em_ego", "$(PLATDEP)/ego/ra$(EXT)": "./ra"}
    | exports,
)
