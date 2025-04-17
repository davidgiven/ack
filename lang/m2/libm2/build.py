from build import PLATS
from build.ab import export
from build.ack import ackclibrary, ackcfile

headers = [
    "Arguments.def",
    "ArraySort.def",
    "ASCII.def",
    "Conversions.def",
    "CSP.def",
    "EM.def",
    "Epilogue.def",
    "InOut.def",
    "MathLib0.def",
    "Mathlib.def",
    "PascalIO.def",
    "Processes.def",
    "random.def",
    "RealConversions.def",
    "RealInOut.def",
    "Semaphores.def",
    "Storage.def",
    "Streams.def",
    "Strings.def",
    "StripUnix.def",
    "Termcap.def",
    "Terminal.def",
    "Traps.def",
    "Unix.def",
    "XXTermcap.def",
]

export(
    name="headers",
    items={f"$(PLATIND)/include/modula2/{k}": f"./{k}" for k in headers},
)

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        plat=plat,
        srcs=[
            "./absd.c",
            "./absi.c",
            "./absl.c",
            "./Arguments.c",
            "./ArraySort.mod",
            "./blockmove.c",
            "./cap.c",
            "./catch.c",
            "./confarray.c",
            "./Conversions.mod",
            "./CSP.mod",
            "./dvi.c",
            "./halt.c",
            "./init.c",
            "./InOut.mod",
            "./load.c",
            "./MathLib0.mod",
            "./Mathlib.mod",
            "./PascalIO.mod",
            "./Processes.mod",
            "./random.mod",
            "./rcka.c",
            "./rcki.c",
            "./rckil.c",
            "./rcku.c",
            "./rckul.c",
            "./RealConversions.mod",
            "./RealInOut.mod",
            "./Semaphores.mod",
            "./sigtrp.c",
            "./stackprio.c",
            "./Storage.mod",
            "./store.c",
            "./StrAss.c",
            "./Streams.mod",
            "./Strings.mod",
            "./SYSTEM.c",
            "./Termcap.mod",
            "./Terminal.mod",
            "./Traps.mod",
            "./ucheck.c",
            "./EM.e",
            "./LtoUset.e",
            "./absf.e",
            "./par_misc.e",
        ],
        deps=[
            ".+headers",
            "lang/m2/include",
            "h",
            f"lang/cem/libcc.ansi+lib_{plat}",
        ],
    )

    ackcfile(name=f"mrt_{plat}", plat=plat, srcs=["./head_m2.e"], deps=["h"])

    suffix = "m" if plat.startswith("em") else "o"
    export(
        name=f"all_{plat}",
        items={
            f"$(PLATIND)/{plat}/libmodula2.a": f".+lib_{plat}",
            f"$(PLATIND)/{plat}/modula2.{suffix}": f".+mrt_{plat}",
        },
        deps=[".+headers"],
    )
