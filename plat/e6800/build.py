from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.cg.build import build_cg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="mc6800")
build_cg(name="cg", arch="mc6800")
build_plat_libs(name="plat_libs", arch="mc6800", plat="e6800")

ackcfile(name="boot", srcs=["./boot.s"], plat="e6800")

export(
    name="tools",
    items={
        "$(PLATDEP)/e6800/as$(EXT)": ".+as",
        "$(PLATDEP)/e6800/cg$(EXT)": ".+cg",
        "$(PLATIND)/descr/e6800": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/e6800/boot.o": ".+boot",
        "$(PLATIND)/e6800/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/e6800/include+all",
    ],
)

