include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/spectrum/include/"..h] = "./"..h
end

addheader("ack/plat.h")
addheader("sys/types.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}
