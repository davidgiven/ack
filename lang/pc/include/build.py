from build.c import clibrary

clibrary(
    name="include",
    hdrs={k: "./" + k for k in ["pc_err.h", "pc_file.h", "pc_math.h", "pc.h"]},
)
