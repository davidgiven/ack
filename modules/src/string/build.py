from build.c import clibrary

clibrary(
    name="string",
    srcs=["./bts2str.c", "./long2str.c"],
    hdrs={"ack_string.h": "./ack_string.h"},
)
