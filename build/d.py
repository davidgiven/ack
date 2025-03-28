from build.ab import (
    Rule,
    Targets,
    TargetsMap,
    filenameof,
    filenamesof,
    flatten,
    simplerule,
    emit,
)
from build.utils import filenamesmatchingof, stripext, collectattrs
from build.toolchain import Toolchain, HostToolchain
from os.path import *

Toolchain.DMD = [
    "$(DMD) -of $[outs[0]] $[ins[0]] $(DFLAGS) $(DLINKFLAGS) $[dflags] $[dlinkflags]"
]
Toolchain.DMDLIB = [
    "$(DMD) -lib -of $[outs[0]] -Hd $[dir] $[ins] $(DFLAGS) $[dflags]"
]

Toolchain.HOSTDMD = [
    "$(HOSTDMD) -of $[outs[0]] $[ins[0]] $(HOSTDFLAGS) $(HOSTDLINKFLAGS) $[dflags] $[dlinkflags]"
]
Toolchain.HOSTDMDLIB = [
    "$(HOSTDMD) -lib -of $[outs[0]] -Hf $[outs[1]] $[ins] $(HOSTDFLAGS) $[dflags]"
]


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


@Rule
def dlibrary(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    dflags=[],
    toolchain=Toolchain,
    label="DMDLIB",
):
    simplerule(
        replaces=self,
        ins=srcs + deps,
        outs=[f"={self.localname}.a", f"={self.localname}.di"],
        commands=toolchain.DMDLIB,
        label=label,
    )


@Rule
def dprogram(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    dflags=[],
    dlinkflags=[],
    toolchain=Toolchain,
    label="DMD",
):
    simplerule(
        replaces=self,
        ins=srcs + deps,
        outs=[f"={self.localname}"],
        deps=deps,
        commands=toolchain.DMD,
        label=label,
    )
