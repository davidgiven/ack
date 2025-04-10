from build.c import clibrary

clibrary(
    name="h",
    hdrs={
        k: f"./{k}"
        for k in [
            "em_arith.h",
            "em_label.h",
            "em_label.h",
            "em_mesX.h",
            "em.h",
            "emO_code.h",
        ]
    },
)
