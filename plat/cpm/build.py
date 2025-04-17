from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from mach.proto.top.build import build_top
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

cflags = ["-DUSE_I80_RSTS"]

build_as(name="as", arch="i80")
build_ncg(name="ncg", arch="i80", cflags=cflags)
build_top(name="top", arch="i80")
build_plat_libs(name="plat_libs", arch="i80", plat="cpm")

ackcfile(name="boot", srcs=["./boot.s"], plat="cpm", cflags=cflags)

export(
    name="tools",
    items={
        "$(PLATDEP)/cpm/as$(EXT)": ".+as",
        "$(PLATDEP)/cpm/ncg$(EXT)": ".+ncg",
        "$(PLATDEP)/cpm/top$(EXT)": ".+top",
        "$(PLATIND)/descr/cpm": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/cpm/boot.o": ".+boot",
        "$(PLATIND)/cpm/libsys.a": "./libsys",
    }
    | exportheaders("./include", prefix="$(PLATIND)/cpm/include"),
    deps=[".+tools", ".+plat_libs", "util/ack+all"],
)
