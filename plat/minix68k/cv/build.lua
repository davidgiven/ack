cprogram {
    name = "cv",
    srcs = { "./cv.c" },
    deps = {
        "h+emheaders",
        "modules/src/data+lib",
        "modules/src/object+lib"
    }
}
