cprogram {
    name = "cv",
    srcs = { "./*.c" },
    deps = {
        "h+emheaders",
        "modules/src/object+lib",
    }
}
