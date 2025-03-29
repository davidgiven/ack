from build.ab import export

PLATS = ["pc86"]

export(
    name="common",
    deps=[
        "util/ack+all",
        "lang/cem/cemcom.ansi+all",
        "lang/cem/cpp.ansi+all",
        "util/amisc+all",
        "util/misc+all",
        "util/arch+all",
        "util/led+all",
        "util/opt+all",
    ],
)

export(
    name="all",
    deps=[
        "plat/pc86+all",
        ".+common",
    ],
)
