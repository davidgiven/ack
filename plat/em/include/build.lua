include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/em/include/"..h] = "./"..h
end

addheader("ack/plat.h")
addheader("sys/types.h")
addheader("sys/timeb.h")
addheader("ack/signal.h")
addheader("sgtty.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}


