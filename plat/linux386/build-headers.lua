include("plat/build.lua")

acklibrary {
	name = "headers",
	hdrs = {
		["ack/config.h"] = "./include/ack/config.h",
		["sys/ioctl.h"] = "./include/sys/ioctl.h",
		["unistd.h"] = "./include/unistd.h",
	}
}

