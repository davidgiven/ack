from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="i80")
build_ncg(name="ncg", arch="i80", cflags=["-DUSE_I80_RSTS"])
build_plat_libs(name="plat_libs", arch="i80", plat="cpm")

ackcfile(name="boot", srcs=["./boot.s"], plat="cpm")

export(
    name="tools",
    items={
        "$(PLATDEP)/cpm/as": ".+as",
        "$(PLATDEP)/cpm/ncg": ".+ncg",
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
