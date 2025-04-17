from build.ab import export
from build.ack import ackcfile
from mach.proto.cg.build import build_cg
from mach.proto.top.build import build_top
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="pdp")
build_cg(name="cg", arch="pdp")
build_top(name="top", arch="pdp")
build_plat_libs(name="plat_libs", arch="pdp", plat="pdpv7")

ackcfile(name="boot", srcs=["./boot.s"], plat="pdpv7")

export(
    name="tools",
    items={
        "$(PLATDEP)/pdpv7/as$(EXT)": ".+as",
        "$(PLATDEP)/pdpv7/cg$(EXT)": ".+cg",
        "$(PLATDEP)/pdpv7/top$(EXT)": ".+top",
        "$(PLATDEP)/pdpv7/cv$(EXT)": "./cv",
        "$(PLATIND)/descr/pdpv7": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/pdpv7/boot.o": ".+boot",
        "$(PLATIND)/pdpv7/libsys.a": "./libsys",
    },
    deps=[".+tools", ".+plat_libs", "util/ack+all", "./include+all"],
)
