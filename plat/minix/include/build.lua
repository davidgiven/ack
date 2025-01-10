include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "plat/minix/include/"..h
	packagemap["$(PLATIND)/minix/include/"..h] = "plat/minix/include/"..h
end

-- addheader("unistd.h")
addheader("ansi.h")
addheader("ack/fcntl.h")
addheader("ack/signal.h")
addheader("ack/limits.h")
addheader("minix/callnr.h")
addheader("minix/const.h")
addheader("minix/type.h")
addheader("minix/com.h")
addheader("utime.h")
addheader("sgtty.h")
addheader("sys/dir.h")
addheader("sys/types.h")
addheader("sys/errno.h")
addheader("sys/stat.h")
addheader("sys/wait.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}
