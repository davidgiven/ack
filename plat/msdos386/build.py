from build.ab import export, simplerule
from build.ack import ackcfile
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="i386")
build_ncg(name="ncg", arch="i386")
build_plat_libs(name="plat_libs", arch="i386", plat="msdos386")

ackcfile(name="boot", srcs=["./boot.s"], plat="msdos386")
ackcfile(
    name="stub", srcs=["./stub.s"], plat="msdos386", deps=["./libsys+headers"]
)

simplerule(
    name="stub_aout",
    ins=["util/led", ".+stub"],
    outs=["=stub.aout"],
    commands=["$[ins[0]] $[ins[1]] -o $[outs[0]]"],
)

simplerule(
    name="stub_exe",
    ins=["util/amisc+aslod", ".+stub_aout"],
    outs=["=stub.exe"],
    commands=["$[ins[0]] $[ins[1]] $[outs[0]]"],
)

export(
    name="tools",
    items={
        "$(PLATDEP)/msdos386/as$(EXT)": ".+as",
        "$(PLATDEP)/msdos386/ncg$(EXT)": ".+ncg",
        "$(PLATIND)/descr/msdos386": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/msdos386/boot.o": ".+boot",
        "$(PLATIND)/msdos386/libsys.a": "./libsys",
        "$(PLATIND)/msdos386/stub.exe": ".+stub_exe",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/msdos386/include+all",
    ],
)
