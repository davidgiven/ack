cprogram {
    name = "cv",
    srcs = { "./cv.c" },
    deps = {
        "h+emheaders",
        "modules/src/object+lib",
    }
}
