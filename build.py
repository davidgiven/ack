from build.ab import export

PLATS=[
    "pc86"
]

export(
    name="common",
    deps=[
        "util/ack+all",
        "lang/cem/cemcom.ansi+all",
        "util/amisc+all",
        "util/arch+all",
        "util/led+all",
    ],
)

export(
    name="all",
    deps=[
        "plat/pc86+all",
        ".+common",
    ],
)
