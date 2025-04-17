from build.ab import export
from build.ack import ackcfile
from mach.proto.mcg.build import build_mcg
from mach.proto.top.build import build_top
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="mips", deps=["mach/mips/as+lib"])
build_mcg(name="mcg", arch="mips")
build_top(name="top", arch="mips")
build_plat_libs(name="plat_libs", arch="mips", plat="linuxmips")

ackcfile(name="boot", srcs=["./boot.s"], plat="linuxmips")

export(
    name="tools",
    items={
        "$(PLATDEP)/linuxmips/as$(EXT)": ".+as",
        "$(PLATDEP)/linuxmips/mcg$(EXT)": ".+mcg",
        "$(PLATDEP)/linuxmips/top$(EXT)": ".+top",
        "$(PLATIND)/descr/linuxmips": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/linuxmips/boot.o": ".+boot",
        "$(PLATIND)/linuxmips/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/linuxmips/include+all",
    ],
)
