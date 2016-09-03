cprogram {
    name = "led",
    srcs = { "./*.c" },
    deps = {
        "modules/src/string+lib",
        "modules/src/object+lib",
        "h+emheaders",
    }
}

installable {
    name = "pkg",
    map = {
        ["$(INSDIR)/share/man/man5/ack.out.5"] = "./ack.out.5",
        ["$(INSDIR)/share/man/man6/led.6"] = "./led.6",
        ["$(PLATDEP)/em_led"] = "+led",
    }
}

