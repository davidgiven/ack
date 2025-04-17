from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="m68k2")
build_ncg(
    name="ncg",
    arch="m68020",
    cflags=["-DWORD_SIZE=2", "-DTBL68000=1"],
)
build_plat_libs(name="plat_libs", arch="m68k2", plat="minix68k")

ackcfile(name="boot", srcs=["./boot.s"], plat="minix68k")

export(
    name="tools",
    items={
        "$(PLATDEP)/minix68k/as$(EXT)": ".+as",
        "$(PLATDEP)/minix68k/ncg$(EXT)": ".+ncg",
        "$(PLATDEP)/minix68k/cv$(EXT)": "./cv",
        "$(PLATIND)/descr/minix68k": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/minix68k/boot.o": ".+boot",
        "$(PLATIND)/minix68k/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "plat/minix68k/include+all",
    ],
)
