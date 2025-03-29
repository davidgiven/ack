from build.ab import export
from build.ack import ackcfile
from mach.proto.ncg.build import build_ncg
import importlib

asm = importlib.import_module("mach.proto.as.build")

asm.build_as(name="as", arch="i86")
build_ncg(name="ncg", arch="i86")

ackcfile(name="boot", srcs=["./boot.s"], plat="pc86")

export(
    name="tools",
    items={
        "$(PLATDEP)/pc86/as": ".+as",
        "$(PLATDEP)/pc86/ncg": ".+ncg",
        "$(PLATIND)/descr/pc86": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/pc86/boot.o": ".+boot",
    },
    deps=[".+tools", "util/ack+all"],
)
