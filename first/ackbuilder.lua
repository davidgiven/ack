-- Targets:
--
-- {
--   dir = target's build directory
--   outs = target's object files
--   is = { set of rule types which made the target }
-- }

local environment = {}
local rules = {}
local targets = {}
local buildfiles = {}
local globals
local cwd = "."

local function subenv(old, cb)
	if not old then
		old = environment
	end
	local new = {}
	setmetatable(new, {__index = old})
	cb(new, old)
	return new
end

local function asstring(o)
	local t = type(o)
	if (t == "string") then
		return o
	elseif (t == "number") then
		return o
	elseif (t == "table") then
		local s = {}
		for _, v in pairs(o) do
			s[#s+1] = asstring(v)
		end
		return table.concat(s, " ")
	else
		error(string.format("can't turn values of type '%s' into strings", t))
	end
end

local function emit(...)
	local n = select("#", ...)
	local args = {...}

	for i=1, n do
		local s = asstring(args[i])
		io.stdout:write(s)
		if not s:find("\n$") then
			io.stdout:write(" ")
		end
	end
end

local function loadbuildfile(filename)
	local data, chunk, e
	data = io.open(filename):read("*a")
	if not e then
		local thisglobals = {_G = thisglobals}
		setmetatable(thisglobals, {__index = globals})
		chunk, e = loadstring(data, filename, "text", thisglobals)
	end
	if e then
		error(string.format("couldn't load '%s': %s", filename, e))
	end

	chunk()
end

local function loadtarget(targetname)
	if targets[target] then
		return targets[targetname]
	end

	local target
	if not target:find(":") then
		target = {
			outs = {targetname},
			is = {
				__implicitfile = true
			}
		}
		targets[targetname] = target
	else
		local _, _, filepart, targetpart = targetname:find("^([^:]+):(%w+)$")
		if not filepart or not targetpart then
			error(string.format("malformed target name '%s'", targetname))
		end
		if not buildfiles[filepart] then
			buildfiles[filepart] = true

			local oldcwd = cwd
			cwd = filepart
			loadbuildfile(filepart.."/build.lua")
			cwd = oldcwd
		end

		target = targets[targetname]
		if not target then
			error(string.format("build file '%s' contains no rule '%s'",
				filepart, targetpart))
		end
	end

	return target
end

local typeconverters = {
	targets = function(propname, i)
		if (type(i) == "string") then
			i = {i}
		elseif (type(i) ~= "table") then
			error(string.format("property '%s' must be a target list", propname))
		end

		local o = {}
		for _, s in ipairs(i) do
			if (type(s) ~= "string") then
				error(string.format("member of target list '%s' is not a string", propname))
			end

			if s:find("^//") then
				s = s:gsub("^//", "")
			elseif s:find("^[/]") then
				s = concatpath(cwd, s)
			end
			o[#o+1] = s
		end
	end,

	strings = function(propname, i)
		if (type(i) == "string") then
			i = {i}
		elseif (type(i) ~= "table") then
			error(string.format("property '%s' must be a string list", propname))
		end

		return i
	end,

	string = function(propname, i)
		if (type(i) ~= "string") then
			error(string.format("property '%s' must be a string", propname))
		end
	end,
}
	
local function definerule(rulename, types, cb)
	if rules[rulename] then
		error(string.format("rule '%s' is already defined", rulename))
	end

	types.name = { type="string" }

	for propname, typespec in pairs(types) do
		if not typeconverters[typespec.type] then
			error(string.format("property '%s' has unrecognised type '%s'",
				propname, typespec.type))
		end
	end

	rules[rulename] = function(e)
		local args = {}
		for propname, typespec in pairs(types) do
			if not e[propname] and not typespec.optional then
				error(string.format("missing mandatory property '%s'", propname))
			end

			args[propname] = typeconverters[typespec.type](propname, e[propname])
			e[propname] = nil
		end

		local propname, _ = next(e)
		if propname then
			error(string.format("don't know what to do with property '%s'", propname))
		end
	end
end

-----------------------------------------------------------------------------
--                              DEFAULT RULES                              --
-----------------------------------------------------------------------------

definerule("simplerule",
	{
		ins = { type="targets" },
		outs = { type="strings" },
	},
	function (e)
	end
)

-----------------------------------------------------------------------------
--                               MAIN PROGRAM                              --
-----------------------------------------------------------------------------

globals = {
	asstring = asstring,
	definerule = definerule,
	emit = emit,
	environment = environment,
}
setmetatable(globals,
	{
		__index = function(self, k)
			local rule = rules[k]
			if rule then
				return rule
			else
				return _G[k]
			end
		end
	}
)

emit("hide=@\n")
for _, file in ipairs({...}) do
	loadbuildfile(file)
end

