include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/rpi/include/"..h] = "./"..h
end

addheader("ack/plat.h")
addheader("pi.h")
addheader("sys/select.h")
addheader("sys/types.h")
addheader("termios.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}


