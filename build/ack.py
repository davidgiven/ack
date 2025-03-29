from build.toolchain import Toolchain
from build.c import cfile, clibrary


class AckToolchain(Toolchain):
    PREFIX = "ACK"
    CC = [
        "ACKDIR=$(INSDIR) $(INSDIR)/bin/ack $(ACKCFLAGS) $[cflags] -m$[plat] -c -o $[outs[0]] $[ins[0]]"
    ]


def ackcfile(name, plat=None, **kwargs):
    assert plat
    kwargs["deps"] = kwargs.get("deps", []) + [
        f"plat/{plat}+tools",
        f"plat/{plat}/include",
        "util/ack+all",
        "util/misc+all",
        "util/opt+all",
        "lang/cem/cemcom.ansi+all",
        "lang/cem/cpp.ansi+all",
    ]
    kwargs["args"] = kwargs.get("args", {}) | {"plat": plat}
    cfile(name=name, toolchain=AckToolchain, **kwargs)


def ackclibrary(name, plat=None, **kwargs):
    assert plat
    kwargs["deps"] = kwargs.get("deps", []) + [
        f"plat/{plat}+tools",
        f"plat/{plat}/include",
        "util/ack+all",
        "util/misc+all",
        "util/opt+all",
        "lang/cem/cemcom.ansi+all",
        "lang/cem/cpp.ansi+all",
    ]
    kwargs["args"] = kwargs.get("args", {}) | {"plat": plat}
    clibrary(name=name, toolchain=AckToolchain, **kwargs)
