acklibrary {
    name = "lib",
    srcs = {
        "./*.s",
        "plat/linux/libsys/*.c",
        "plat/linux/libsys/*.s",
    },
    vars = {
        plat = "linux68k"
    }
}

