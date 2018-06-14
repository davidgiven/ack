include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/em22/include/"..h] = "./"..h
end

addheader("ack/config.h")
addheader("sys/types.h")
addheader("sys/timeb.h")
addheader("unistd.h")
addheader("sgtty.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}


