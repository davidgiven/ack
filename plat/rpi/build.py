from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="vc4", deps=["mach/vc4/as+lib"])
build_ncg(name="ncg", arch="vc4")
build_plat_libs(name="plat_libs", arch="vc4", plat="rpi")

ackcfile(name="boot", srcs=["./boot.s"], plat="rpi")

export(
    name="tools",
    items={
        "$(PLATDEP)/rpi/as$(EXT)": ".+as",
        "$(PLATDEP)/rpi/ncg$(EXT)": ".+ncg",
        "$(PLATIND)/descr/rpi": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/rpi/boot.o": ".+boot",
        "$(PLATIND)/rpi/libsys.a": "./libsys",
    }
    | exportheaders("./include", prefix="$(PLATIND)/rpi/include"),
    deps=[".+tools", ".+plat_libs", "util/ack+all"],
)
