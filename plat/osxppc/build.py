from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from mach.proto.top.build import build_top
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="powerpc")
build_ncg(name="ncg", arch="powerpc")
build_top(name="top", arch="powerpc")
build_plat_libs(name="plat_libs", arch="powerpc", plat="osxppc")

ackcfile(name="boot", srcs=["./boot.s"], plat="osxppc")

export(
    name="tools",
    items={
        "$(PLATDEP)/osxppc/as$(EXT)": ".+as",
        "$(PLATDEP)/osxppc/ncg$(EXT)": ".+ncg",
        "$(PLATDEP)/osxppc/top$(EXT)": ".+top",
        "$(PLATIND)/descr/osxppc": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/osxppc/boot.o": ".+boot",
        "$(PLATIND)/osxppc/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/osxppc/include+all",
        "plat/osx/cvmach+all",
    ],
)
