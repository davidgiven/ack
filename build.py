from build.ab import export

PLATS = ["pc86"]

# This contains the platform-independent host tooling required to build the plats.
export(
    name="common",
    deps=[
        "lang/cem/cemcom.ansi+all",
        "lang/cem/cpp.ansi+all",
        "lang/basic/src+all",
        "util/ack+all",
        "util/amisc+all",
        "util/arch+all",
        "util/led+all",
        "util/misc+all",
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
