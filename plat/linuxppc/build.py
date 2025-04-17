from build.ab import export
from build.ack import ackcfile
from mach.proto.ncg.build import build_ncg
from mach.proto.top.build import build_top
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="powerpc")
build_ncg(name="ncg", arch="powerpc")
build_top(name="top", arch="powerpc")
build_plat_libs(name="plat_libs", arch="powerpc", plat="linuxppc")

ackcfile(name="boot", srcs=["./boot.s"], plat="linuxppc")

export(
    name="tools",
    items={
        "$(PLATDEP)/linuxppc/as$(EXT)": ".+as",
        "$(PLATDEP)/linuxppc/ncg$(EXT)": ".+ncg",
        "$(PLATDEP)/linuxppc/top$(EXT)": ".+top",
        "$(PLATIND)/descr/linuxppc": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/linuxppc/boot.o": ".+boot",
        "$(PLATIND)/linuxppc/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/linuxppc/include+all",
    ],
)
