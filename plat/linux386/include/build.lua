include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/linux386/include/"..h] = "./"..h
end

addheader("ack/plat.h")
addheader("ack/fcntl.h")
addheader("ack/signal.h")
addheader("sys/ioctl.h")
addheader("sys/types.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}

