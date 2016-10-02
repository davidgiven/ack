include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "plat/osx/include/"..h
	packagemap["$(PLATIND)/osx386/include/"..h] = "plat/osx/include/"..h
end

addheader("ack/config.h")
addheader("sys/mman.h")
addheader("sys/types.h")
addheader("unistd.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}
