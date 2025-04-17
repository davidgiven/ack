from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="i386")
build_ncg(name="ncg", arch="i386")
build_plat_libs(name="plat_libs", arch="i386", plat="linux386")

ackcfile(name="boot", srcs=["./boot.s"], plat="linux386")

export(
    name="tools",
    items={
        "$(PLATDEP)/linux386/as$(EXT)": ".+as",
        "$(PLATDEP)/linux386/ncg$(EXT)": ".+ncg",
        "$(PLATIND)/descr/linux386": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/linux386/boot.o": ".+boot",
        "$(PLATIND)/linux386/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/linux386/include+all",
    ],
)
