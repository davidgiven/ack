include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "plat/linux/include/"..h
	packagemap["$(PLATIND)/linux/include/"..h] = "plat/linux/include/"..h
end

addheader("ack/plat.h")
addheader("ack/fcntl.h")
addheader("ack/signal.h")
addheader("sys/ioctl.h")
addheader("sys/types.h")
addheader("sys/wait.h")


acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}
