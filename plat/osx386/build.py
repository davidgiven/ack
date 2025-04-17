from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="i386")
build_ncg(name="ncg", arch="i386")
build_plat_libs(name="plat_libs", arch="i386", plat="osx386")

ackcfile(name="boot", srcs=["./boot.s"], plat="osx386")

export(
    name="tools",
    items={
        "$(PLATDEP)/osx386/as$(EXT)": ".+as",
        "$(PLATDEP)/osx386/ncg$(EXT)": ".+ncg",
        "$(PLATIND)/descr/osx386": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/osx386/boot.o": ".+boot",
        "$(PLATIND)/osx386/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/osx386/include+all",
        "plat/osx/cvmach+all",
    ],
)
