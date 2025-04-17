from build.ab import export
from build.ack import ackcfile
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="i86")
build_ncg(name="ncg", arch="i86")
build_plat_libs(name="plat_libs", arch="i86", plat="msdos86")

ackcfile(name="boot", srcs=["./boot.s"], plat="msdos86")

export(
    name="tools",
    items={
        "$(PLATDEP)/msdos86/as$(EXT)": ".+as",
        "$(PLATDEP)/msdos86/ncg$(EXT)": ".+ncg",
        "$(PLATIND)/descr/msdos86": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/msdos86/boot.o": ".+boot",
        "$(PLATIND)/msdos86/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/msdos86/include+all",
    ],
)
