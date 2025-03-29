from build.c import clibrary

clibrary(
    name="include", hdrs={"libm2.h": "./libm2.h", "m2_traps.h": "./m2_traps.h"}
)
