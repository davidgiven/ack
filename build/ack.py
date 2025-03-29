from build.toolchain import Toolchain
from build.c import cfile


class AckToolchain(Toolchain):
    PREFIX = "ACK"
    CC = ["$(INSDIR)/bin/ack $(ACKCFLAGS) -m$[plat] -c -o $[outs[0]] $[ins[0]]"]


def ackcfile(name, plat=None, **kwargs):
    kwargs["deps"] = kwargs.get("deps", []) + [
        f"plat/{plat}+tools",
        "util/ack+all",
    ]
    kwargs["args"] = kwargs.get("args", {}) | {"plat": plat}
    cfile(name=name, toolchain=AckToolchain, **kwargs)
