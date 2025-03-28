from build.ab import export

export(
    name="all",
    deps=[
        "util/amisc+all",
        "util/arch+all",
        "util/led+all",
        "util/ncgg",
        "util/LLgen",
        "lang/cem/cemcom-ansi+all",
    ],
)
