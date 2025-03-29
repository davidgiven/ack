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

emit(
    """
ifeq ($(OSX),no)
STARTGROUP ?= -Wl,--start-group
ENDGROUP ?= -Wl,--end-group
endif
"""
)

Toolchain.CC = ["$(CC) -c -o $[outs[0]] $[ins[0]] $(CFLAGS) $[cflags]"]
Toolchain.CPP = ["$(CC) -E -P -o $[outs] $[cflags] -x c $[ins]"]
Toolchain.CXX = ["$(CXX) -c -o $[outs[0]] $[ins[0]] $(CFLAGS) $[cflags]"]
Toolchain.AR = ["$(AR) cqs $[outs[0]] $[ins]"]
Toolchain.ARXX = ["$(AR) cqs $[outs[0]] $[ins]"]
Toolchain.CLINK = [
    "$(CC) -o $[outs[0]] $(STARTGROUP) $[ins] $[ldflags] $(LDFLAGS) $(ENDGROUP)"
]
Toolchain.CXXLINK = [
    "$(CXX) -o $[outs[0]] $(STARTGROUP) $[ins] $[ldflags] $(LDFLAGS) $(ENDGROUP)"
]


HostToolchain.CC = [
    "$(HOSTCC) -c -o $[outs[0]] $[ins[0]] $(HOSTCFLAGS) $[cflags]"
]
HostToolchain.CPP = ["$(HOSTCC) -E -P -o $[outs] $[cflags] -x c $[ins]"]
HostToolchain.CXX = [
    "$(HOSTCXX) -c -o $[outs[0]] $[ins[0]] $(HOSTCFLAGS) $[cflags]"
]
HostToolchain.AR = ["$(HOSTAR) cqs $[outs[0]] $[ins]"]
HostToolchain.ARXX = ["$(HOSTAR) cqs $[outs[0]] $[ins]"]
HostToolchain.CLINK = [
    "$(HOSTCC) -o $[outs[0]] $(STARTGROUP) $[ins] $[ldflags] $(HOSTLDFLAGS) $(ENDGROUP)"
]
HostToolchain.CXXLINK = [
    "$(HOSTCXX) -o $[outs[0]] $(STARTGROUP) $[ins] $[ldflags] $(HOSTLDFLAGS) $(ENDGROUP)"
]


def is_source_file(f):
    return (
        f.endswith(".c")
        or f.endswith(".cc")
        or f.endswith(".cpp")
        or f.endswith(".S")
        or f.endswith(".s")
        or f.endswith(".m")
        or f.endswith(".mm")
    )


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


def cfileimpl(self, name, srcs, deps, suffix, commands, label, cflags):
    outleaf = "=" + stripext(basename(filenameof(srcs[0]))) + suffix

    hdr_deps = _indirect(deps, "cheader_deps")
    cflags = collectattrs(
        targets=hdr_deps, name="caller_cflags", initial=cflags
    )

    t = simplerule(
        replaces=self,
        ins=srcs,
        deps=sorted(_indirect(hdr_deps, "cheader_files")),
        outs=[outleaf],
        label=label,
        commands=commands,
        args={"cflags": cflags},
    )


@Rule
def cfile(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    cflags=[],
    suffix=".o",
    toolchain=Toolchain,
    label="CC",
):
    cfileimpl(
        self,
        name,
        srcs,
        deps,
        suffix,
        toolchain.CC,
        toolchain.PREFIX + label,
        cflags,
    )


@Rule
def cxxfile(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    cflags=[],
    suffix=".o",
    toolchain=Toolchain,
    label="CXX",
):
    cfileimpl(
        self,
        name,
        srcs,
        deps,
        suffix,
        toolchain.CXX,
        toolchain.PREFIX + label,
        cflags,
    )


def _removeprefix(self, prefix):
    if self.startswith(prefix):
        return self[len(prefix) :]
    else:
        return self[:]


def findsources(name, srcs, deps, cflags, filerule, toolchain, cwd):
    for f in filenamesof(srcs):
        if not is_source_file(f):
            cflags = cflags + [f"-I{dirname(f)}"]
            deps = deps + [f]

    objs = []
    for s in flatten(srcs):
        objs += [
            filerule(
                name=join(name, _removeprefix(f, "$(OBJ)/")),
                srcs=[f],
                deps=deps,
                cflags=sorted(set(cflags)),
                toolchain=toolchain,
                cwd=cwd,
            )
            for f in filenamesof([s])
            if is_source_file(f)
        ]
        if any(f.endswith(".o") for f in filenamesof([s])):
            objs += [s]

    return objs


def libraryimpl(
    self,
    name,
    srcs,
    deps,
    hdrs,
    caller_cflags,
    caller_ldflags,
    cflags,
    ldflags,
    toolchain,
    commands,
    label,
    filerule,
):
    hdr_deps = _combine(_indirect(deps, "cheader_deps"), [self])
    lib_deps = _combine(_indirect(deps, "clibrary_deps"), [self])

    hr = None
    hf = []
    ar = None
    if hdrs:
        cs = []
        ins = hdrs.values()
        outs = []
        i = 0
        for dest, src in hdrs.items():
            s = filenamesof([src])
            assert (
                len(s) == 1
            ), "the target of a header must return exactly one file"

            cs += [f"$(CP) $[ins[{i}]] $[outs[{i}]]"]
            outs += ["=" + dest]
            i = i + 1

        hr = simplerule(
            name=f"{self.localname}_hdr",
            ins=ins,
            outs=outs,
            commands=cs,
            label=toolchain.PREFIX + "CHEADERS",
        )
        hr.materialise()
        hf = [f"-I{hr.dir}"]

    if srcs:
        objs = findsources(
            self.localname,
            srcs,
            deps + ([hr] if hr else []),
            cflags + hf,
            filerule,
            toolchain,
            self.cwd,
        )

        ar = simplerule(
            name=f"{self.localname}_lib",
            ins=objs,
            outs=[f"={self.localname}.a"],
            label=label,
            commands=commands,
        )
        ar.materialise()

    self.outs = ([hr] if hr else []) + ([ar] if ar else [])
    self.deps = self.outs
    self.args["cheader_deps"] = hdr_deps
    self.args["clibrary_deps"] = lib_deps
    self.args["cheader_files"] = [hr] if hr else []
    self.args["clibrary_files"] = [ar] if ar else []
    self.args["caller_cflags"] = caller_cflags + hf
    self.args["caller_ldflags"] = caller_ldflags


@Rule
def clibrary(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    hdrs: TargetsMap = None,
    caller_cflags=[],
    caller_ldflags=[],
    cflags=[],
    ldflags=[],
    toolchain=Toolchain,
    label="LIB",
    cfilerule=cfile,
):
    libraryimpl(
        self,
        name,
        srcs,
        deps,
        hdrs,
        caller_cflags,
        caller_ldflags,
        cflags,
        ldflags,
        toolchain,
        toolchain.AR,
        toolchain.PREFIX + label,
        cfilerule,
    )


@Rule
def hostclibrary(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    hdrs: TargetsMap = None,
    caller_cflags=[],
    caller_ldflags=[],
    cflags=[],
    ldflags=[],
    toolchain=HostToolchain,
    label="LIB",
    cfilerule=cfile,
):
    libraryimpl(
        self,
        name,
        srcs,
        deps,
        hdrs,
        caller_cflags,
        caller_ldflags,
        cflags,
        ldflags,
        toolchain,
        toolchain.AR,
        toolchain.PREFIX + label,
        cfilerule,
    )


@Rule
def cxxlibrary(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    hdrs: TargetsMap = None,
    caller_cflags=[],
    caller_ldflags=[],
    cflags=[],
    ldflags=[],
    toolchain=Toolchain,
    label="CXXLIB",
    cxxfilerule=cxxfile,
):
    libraryimpl(
        self,
        name,
        srcs,
        deps,
        hdrs,
        caller_cflags,
        caller_ldflags,
        cflags,
        ldflags,
        toolchain,
        toolchain.ARXX,
        toolchain.PREFIX + label,
        cxxfilerule,
    )


@Rule
def hostcxxlibrary(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    hdrs: TargetsMap = None,
    caller_cflags=[],
    caller_ldflags=[],
    cflags=[],
    ldflags=[],
    toolchain=HostToolchain,
    label="CXXLIB",
    cxxfilerule=cxxfile,
):
    libraryimpl(
        self,
        name,
        srcs,
        deps,
        hdrs,
        caller_cflags,
        caller_ldflags,
        cflags,
        ldflags,
        toolchain,
        toolchain.ARXX,
        toolchain.PREFIX + label,
        cxxfilerule,
    )


def programimpl(
    self,
    name,
    srcs,
    deps,
    cflags,
    ldflags,
    toolchain,
    commands,
    label,
    filerule,
):
    cfiles = findsources(
        self.localname, srcs, deps, cflags, filerule, toolchain, self.cwd
    )

    lib_deps = []
    for d in deps:
        lib_deps = _combine(lib_deps, d.args.get("clibrary_deps", {d}))
    libs = filenamesmatchingof(lib_deps, "*.a")
    ldflags = collectattrs(
        targets=lib_deps, name="caller_ldflags", initial=ldflags
    )

    simplerule(
        replaces=self,
        ins=cfiles + libs,
        outs=[f"={self.localname}$(EXT)"],
        deps=_indirect(lib_deps, "clibrary_files"),
        label=label,
        commands=commands,
        args={
            "ldflags": collectattrs(
                targets=lib_deps, name="caller_ldflags", initial=ldflags
            )
        },
    )


@Rule
def cprogram(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    cflags=[],
    ldflags=[],
    toolchain=Toolchain,
    label="CLINK",
    cfilerule=cfile,
):
    programimpl(
        self,
        name,
        srcs,
        deps,
        cflags,
        ldflags,
        toolchain,
        toolchain.CLINK,
        toolchain.PREFIX + label,
        cfilerule,
    )


@Rule
def hostcprogram(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    cflags=[],
    ldflags=[],
    toolchain=HostToolchain,
    label="CLINK",
    cfilerule=cfile,
):
    programimpl(
        self,
        name,
        srcs,
        deps,
        cflags,
        ldflags,
        toolchain,
        toolchain.CLINK,
        toolchain.PREFIX + label,
        cfilerule,
    )


@Rule
def cxxprogram(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    cflags=[],
    ldflags=[],
    toolchain=Toolchain,
    label="CXXLINK",
    cxxfilerule=cxxfile,
):
    programimpl(
        self,
        name,
        srcs,
        deps,
        cflags,
        ldflags,
        toolchain,
        toolchain.CXXLINK,
        toolchain.PREFIX + label,
        cxxfilerule,
    )


@Rule
def hostcxxprogram(
    self,
    name,
    srcs: Targets = None,
    deps: Targets = None,
    cflags=[],
    ldflags=[],
    toolchain=HostToolchain,
    label="CXXLINK",
    cxxfilerule=cxxfile,
):
    programimpl(
        self,
        name,
        srcs,
        deps,
        cflags,
        ldflags,
        toolchain,
        toolchain.CXXLINK,
        toolchain.PREFIX + label,
        cxxfilerule,
    )


def _cppfileimpl(self, name, srcs, deps, cflags, toolchain):
    hdr_deps = _indirect(deps, "cheader_deps")
    cflags = collectattrs(
        targets=hdr_deps, name="caller_cflags", initial=cflags
    )

    simplerule(
        replaces=self,
        ins=srcs,
        outs=[f"={self.localname}"],
        deps=deps,
        commands=toolchain.CPP,
        args={"cflags": cflags},
        label=toolchain.PREFIX + "CPPFILE",
    )


@Rule
def cppfile(
    self,
    name,
    srcs: Targets = [],
    deps: Targets = [],
    cflags=[],
    toolchain=Toolchain,
):
    _cppfileimpl(self, name, srcs, deps, cflags, toolchain)


@Rule
def hostcppfile(
    self,
    name,
    srcs: Targets = [],
    deps: Targets = [],
    cflags=[],
    toolchain=HostToolchain,
):
    _cppfileimpl(self, name, srcs, deps, cflags, toolchain)
