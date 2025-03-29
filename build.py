from build.ab import export

PLATS = ["pc86", "cpm"]

# This contains the platform-independent host tooling required to build the plats.
export(
    name="common",
    deps=[
        "lang/cem/cemcom.ansi+all",
        "lang/cem/cpp.ansi+all",
        "lang/basic/src+all",
        "lang/m2/comp+all",
        "lang/pc/comp+all",
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
    deps=(
        [
            ".+common",
        ]
        + [f"plat/{p}+all" for p in PLATS]
    ),
)
