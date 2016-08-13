include("plat/build.lua")

acklibrary {
	name = "headers",
	hdrs = {
		["ack/config.h"] = "./include/ack/config.h",
		["unistd.h"] = "./include/unistd.h",
	}
}

