include("plat/build.lua")

local headers = {}
local installmap = {}

local function addheader(dir, list)
	for _, f in ipairs(list) do
		local b = basename(f)
		headers[dir..b] = f
		installmap[concatpath("$(PLATIND)/include/ansi/", dir, b)] = f
	end
end

addheader("", filenamesof(
		"./assert.h",
		"./ctype.h",
		"./errno.h",
		"./fcntl.h",
		"./float.h",
		"./iso646.h",
		"./limits.h",
		"./locale.h",
		"./math.h",
		"./setjmp.h",
		"./signal.h",
		"./stdarg.h",
		"./stdbool.h",
		"./stddef.h",
		"./stdint.h",
		"./stdio.h",
		"./stdlib.h",
		"./string.h",
		"./time.h",
		"./unistd.h"
	))

addheader("sys/", filenamesof(
		"./sys/time.h",
		"./sys/times.h"
	))

addheader("ack/", filenamesof(
		"./ack/config.h",
		"./ack/emufile.h"
	))

acklibrary {
	name = "headers",
	hdrs = headers
}

installable {
	name = "pkg",
	map = installmap
}
