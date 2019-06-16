include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/cpm/include/"..h] = "./"..h
end

addheader("ack/plat.h")
addheader("ack/file.h")
addheader("sys/types.h")
addheader("cpm.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}


