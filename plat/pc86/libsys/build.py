from build.ack import ackclibrary

ackclibrary(
    name="libsys",
    plat="pc86",
    srcs=[
        "./brk.c",
        "./close.c",
        "./creat.c",
        "./errno.s",
        "./getpid.c",
        "./_hol0.s",
        "./isatty.c",
        "./kill.c",
        "./lseek.c",
        "./open.c",
        "./read.c",
        "./signal.c",
        "./_sys_rawread.s",
        "./_sys_rawwrite.s",
        "./time.c",
        "./write.c",
        "./libsys.h",
    ],
    deps=["lang/cem/libcc.ansi/headers", "plat/pc86/include"],
)
