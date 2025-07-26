from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="m68020")
build_ncg(
    name="ncg",
    arch="m68020",
    cflags=["-DWORD_SIZE=4", "-DTBL68020=1", "-DTBL68881=1"],
)
build_plat_libs(name="plat_libs", arch="m68020", plat="linux68k")

ackcfile(name="boot", srcs=["./boot.s"], plat="linux68k")

export(
    name="tools",
    items={
        "$(PLATDEP)/linux68k/as$(EXT)": ".+as",
        "$(PLATDEP)/linux68k/ncg$(EXT)": ".+ncg",
        "$(PLATIND)/descr/linux68k": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/linux68k/boot.o": ".+boot",
        "$(PLATIND)/linux68k/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/linux68k/include+all",
    ],
)
