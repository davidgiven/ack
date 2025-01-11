include("plat/build.lua")

headermap = {
	"plat/minix/include+headers",
}
packagemap = {
	"plat/minix/include+pkg",
}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/minix68k/include/"..h] = "./"..h
end

addheader("ack/plat.h")
addheader("minix/config.h")
addheader("a.out.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}
