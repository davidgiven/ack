from build.ab import targetof, filenameof
from build.toolchain import Toolchain
from build.c import cfile, clibrary, cprogram
from os.path import *


class AckToolchain(Toolchain):
    PREFIX = "ACK"
    CC = [
        "ACKDIR=$(INSDIR) $(INSDIR)/bin/ack $(ACKCFLAGS) $[cflags] -m$[plat] -c -o $[outs[0]] $[ins[0]]"
    ]
    CLINK = [
        "ACKDIR=$(INSDIR) $(INSDIR)/bin/ack $(ACKLDFLAGS) $[ldflags] -m$[plat] -.$[lang] -o $[outs[0]] $[ins[0]]"
    ]
    AR = ["$(INSDIR)/bin/aal qc $[outs] $[ins]"]

    def is_source_file(f):
        return (
            f.endswith(".c")
            or f.endswith(".S")
            or f.endswith(".s")
            or f.endswith(".e")
            or f.endswith(".mod")
            or f.endswith(".bas")
            or f.endswith(".p")
        )


def ackcfile(name, plat=None, **kwargs):
    assert plat
    kwargs["deps"] = kwargs.get("deps", []) + [
        f"plat/{plat}+tools",
        f"plat/{plat}/include",
        "+common",
    ]
    kwargs["args"] = kwargs.get("args", {}) | {"plat": plat}
    return cfile(name=name, toolchain=AckToolchain, **kwargs)


def ackclibrary(name, plat=None, **kwargs):
    assert plat
    kwargs["deps"] = kwargs.get("deps", []) + [
        f"plat/{plat}+tools",
        f"plat/{plat}/include",
        "+common",
    ]
    kwargs["args"] = kwargs.get("args", {}) | {"plat": plat}
    return clibrary(name=name, toolchain=AckToolchain, **kwargs)


def ackcprogram(name, lang, plat=None, **kwargs):
    assert plat
    kwargs["deps"] = kwargs.get("deps", []) + [
        f"plat/{plat}+all",
        "+common",
    ]
    kwargs["args"] = kwargs.get("args", {}) | {"plat": plat, "lang": lang}
    return cprogram(name=name, toolchain=AckToolchain, **kwargs)


def _combine(list1, list2):
    r = list(list1)
    for i in list2:
        if i not in r:
            r.append(i)
    return r


def _indirect(deps, name):
    r = []
    for d in deps:
        r = _combine(r, d.args.get(name, [d]))
    return r


def exportheaders(lib, prefix=""):
    lib = targetof(lib)
    hh = {}
    for h in sorted(_indirect([lib], "cheader_files")):
        for f in h.outs:
            r = relpath(filenameof(f), h.dir)
            hh[join(prefix, r)] = f
    return hh
