from build.c import hostclibrary, hostcprogram

hostclibrary(
    name="x86emu",
    srcs=[
        "./x86emu/debug.c",
        "./x86emu/decode.c",
        "./x86emu/fpu.c",
        "./x86emu/ops2.c",
        "./x86emu/ops.c",
        "./x86emu/prim_ops.c",
        "./x86emu/sys.c",
    ],
    hdrs={
        "x86emu.h": "./x86emu/x86emu.h",
        "x86emu/debug.h": "./x86emu/x86emu/debug.h",
        "x86emu/decode.h": "./x86emu/x86emu/decode.h",
        "x86emu/fpu_regs.h": "./x86emu/x86emu/fpu_regs.h",
        "x86emu/fpu.h": "./x86emu/x86emu/fpu.h",
        "x86emu/ops.h": "./x86emu/x86emu/ops.h",
        "x86emu/prim_asm.h": "./x86emu/x86emu/prim_asm.h",
        "x86emu/prim_ops.h": "./x86emu/x86emu/prim_ops.h",
        "x86emu/prim_x86_gcc.h": "./x86emu/x86emu/prim_x86_gcc.h",
        "x86emu/regs.h": "./x86emu/x86emu/regs.h",
        "x86emu/types.h": "./x86emu/x86emu/types.h",
        "x86emu/x86emui.h": "./x86emu/x86emu/x86emui.h",
    },
    cflags=["-DDEBUG"],
)

hostcprogram(
    name="emu",
    srcs=["./main.c"],
    cflags=["-DDEBUG"],
    deps=[".+x86emu"],
)
