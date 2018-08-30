acklibrary {
    name = "lib",
    srcs = {
        "./*.c",
        "./*.s",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/pdpv7/include+headers",
	},
    vars = {
        plat = "pdpv7"
    }
}

-- /tmp/ack-build/staging/lib/ack/cpp.ansi -D__unix -I/tmp/ack-build/obj/lang/cem/libcc.ansi/headers/headers -I/tmp/ack-build/obj/plat/rpi/include/headers -D__vc4 -D__rpi -D__ACK -I/tmp/ack-build/staging/share/ack/rpi/include -I/tmp/ack-build/staging/share/ack/include/ansi -Dvc4 -DEM_WSIZE=4 -DEM_PSIZE=4 -DEM_SSIZE=2 -DEM_LSIZE=4 -DEM_FSIZE=4 -DEM_DSIZE=4 -D_EM_WSIZE=4 -D_EM_PSIZE=4 -D_EM_SSIZE=2 -D_EM_LSIZE=4 -D_EM_FSIZE=4 -D_EM_DSIZE=4 plat/rpi/libsys/pi_user_to_phys.s
