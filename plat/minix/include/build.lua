include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "plat/minix/include/"..h
	packagemap["$(PLATIND)/minix/include/"..h] = "plat/minix/include/"..h
end

addheader("unistd.h")
addheader("ansi.h")
addheader("minix/callnr.h")
addheader("minix/const.h")
addheader("minix/config.h")
addheader("minix/type.h")
addheader("minix/com.h")
addheader("utime.h")
addheader("sgtty.h")
addheader("sys/types.h")
addheader("sys/errno.h")
addheader("sys/times.h")
addheader("sys/stat.h")
addheader("sys/wait.h")
addheader("signal.h")
addheader("limits.h")
addheader("fcntl.h")
addheader("time.h")
addheader("lib.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}
