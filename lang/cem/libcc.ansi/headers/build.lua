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

addheader("", filenamesof("./*.h"))
addheader("sys/", filenamesof("./sys/*.h"))

acklibrary {
	name = "headers",
	hdrs = headers
}

installable {
	name = "pkg",
	map = installmap
}
	
