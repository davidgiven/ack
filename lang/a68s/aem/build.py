from build.ab import export, simplerule
from build.pascal import pascalprogram
from lang.a68s.util.build import tailor

tnos = [
    101,
    2,
    103,
    104,
    105,
    111,
    122,
    123,
    24,
    125,
    32,
    133,
    41,
    42,
    150,
    151,
    152,
    153,
    154,
    155,
    161,
    112,  # 16 bit off
    13,  # 32 bit on
    119,  # 16 bit with 32 bit pointers off
]

tailor(
    name="a68sdec0.h",
    srcs=["./a68sdec.p"],
    numbers=tnos + [70, 171, 172, 73, 174, 175, 176, 177, 178, 300],
)

pascalprogram(
    name="init1_out",
    srcs=[
        simplerule(
            name="init1_out_src",
            ins=[
                ".+a68sdec0.h",
                "./freepascal.p",
                tailor(
                    name="a68sint_300",
                    srcs=["./a68sint.p"],
                    numbers=tnos + [300],
                ),
                tailor(
                    name="a68sdum_83_300",
                    srcs=["./a68sdum.p"],
                    numbers=tnos + [83, 300],
                ),
                tailor(
                    name="a68sin_81_83_184_300",
                    srcs=["./a68sin.p"],
                    numbers=tnos + [81, 83, 184, 300],
                ),
                "./end.p",
            ],
            outs=["=init1_out.pas"],
            commands=["cat $[ins] > $[outs]"],
        )
    ],
    fpcflags=["-Miso"],
)

export(name="all", deps=[".+init1_out"])
