from build.ab import export, simplerule
from build.ack import ackcfile
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_plat_libs(name="plat_libs", arch="em22", plat="em22", is_em=True)

export(
    name="tools",
    items={"$(PLATIND)/descr/em22": "./descr"},
    deps=["util/ass"],
)

export(
    name="all",
    items={
        "$(PLATIND)/em22/libsys.a": "./libsys",
    },
    deps=[
        ".+tools",
        ".+plat_libs",
        "util/ack+all",
        "util/ass+all",
        "plat/em22/include+all",
    ],
)
