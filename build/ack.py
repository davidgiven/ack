from build.toolchain import Toolchain
from build.c import cfile, clibrary


class AckToolchain(Toolchain):
    PREFIX = "ACK"
    CC = [
        "ACKDIR=$(INSDIR) $(INSDIR)/bin/ack $(ACKCFLAGS) $[cflags] -m$[plat] -c -o $[outs[0]] $[ins[0]]"
    ]

    def is_source_file(f):
        return (
            f.endswith(".c")
            or f.endswith(".S")
            or f.endswith(".s")
            or f.endswith(".e")
            or f.endswith(".mod")
            or f.endswith(".p")
            or f.endswith(".b")
        )


def ackcfile(name, plat=None, **kwargs):
    assert plat
    kwargs["deps"] = kwargs.get("deps", []) + [
        f"plat/{plat}+tools",
        f"plat/{plat}/include",
        "+common",
    ]
    kwargs["args"] = kwargs.get("args", {}) | {"plat": plat}
    cfile(name=name, toolchain=AckToolchain, **kwargs)


def ackclibrary(name, plat=None, **kwargs):
    assert plat
    kwargs["deps"] = kwargs.get("deps", []) + [
        f"plat/{plat}+tools",
        f"plat/{plat}/include",
        "+common",
    ]
    kwargs["args"] = kwargs.get("args", {}) | {"plat": plat}
    clibrary(name=name, toolchain=AckToolchain, **kwargs)
