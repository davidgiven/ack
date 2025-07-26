from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="i86")
build_ncg(name="ncg", arch="i86")
build_plat_libs(name="plat_libs", arch="i86", plat="pc86")

ackcfile(name="boot", srcs=["./boot.s"], plat="pc86")

export(
    name="tools",
    items={
        "$(PLATDEP)/pc86/as$(EXT)": ".+as",
        "$(PLATDEP)/pc86/ncg$(EXT)": ".+ncg",
        "$(PLATIND)/descr/pc86": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/pc86/boot.o": ".+boot",
        "$(PLATIND)/pc86/libsys.a": "./libsys",
    }
    | exportheaders("./include", prefix="$(PLATIND)/pc86/include"),
    deps=[".+tools", ".+plat_libs", "util/ack+all"],
)
