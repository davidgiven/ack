local posix = require("posix")

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
	if (t == "nil") then
		return ""
	elseif (t == "string") then
		return o
	elseif (t == "number") then
		return o
	elseif (t == "table") then
		if o.is then
			return asstring(o.outs)
		else
			local s = {}
			for _, v in pairs(o) do
				s[#s+1] = asstring(v)
			end
			return table.concat(s, " ")
		end
	else
		error(string.format("can't turn values of type '%s' into strings", t))
	end
end

local function concatpath(...)
	local p = table.concat({...}, "/")
	return p:gsub("/+", "/"):gsub("^%./", "")
end

local function dirname(filename)
	local _, _, b = filename:find("^(.*)/[^/]*$")
	if not b then
		return ""
	end
	return b
end

local function dirnames(collection)
	local o = {}
	for _, s in pairs(collection) do
		local b = dirname(s)
		if (b ~= "") then
			o[#o+1] = b
		end
	end
	return o
end

local function filenamesof(results)
	local f = {}
	for _, r in pairs(results) do
		if (type(r) == "string") then
			f[#f+1] = r
		elseif (type(r) == "table") then
			if r.is and r.outs then
				for _, o in pairs(r.outs) do
					f[#f+1] = o
				end
			end
		end
	end
	return f
end

local function uniquify(collection)
	local s = {}
	local o = {}
	for _, v in pairs(collection) do
		if not s[v] then
			s[v] = true
			o[#o+1] = v
		end
	end
	return o
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

local function templateexpand(list, vars)
	setmetatable(vars, { __index = globals })

	local o = {}
	for _, s in ipairs(list) do
		o[#o+1] = s:gsub("%%%b{}",
			function(expr)
				expr = expr:sub(3, -2)
				local chunk, e = loadstring("return "..expr, expr, "text", vars)
				if e then
					error(string.format("error evaluating expression: %s", e))
				end
				return asstring(chunk())
			end
		)
	end
	return o
end
	
local function loadbuildfile(filename)
	if not buildfiles[filename] then
		buildfiles[filename] = true

		local fp, data, chunk, e
		io.stderr:write("loading ", filename, "\n")
		fp, e = io.open(filename)
		if not e then
			data, e = fp:read("*a")
			fp:close()
			if not e then
				local thisglobals = {_G = thisglobals}
				setmetatable(thisglobals, {__index = globals})
				chunk, e = loadstring(data, filename, "text", thisglobals)
			end
		end
		if e then
			error(string.format("couldn't load '%s': %s", filename, e))
		end

		local oldcwd = cwd
		cwd = dirname(filename)
		chunk()
		cwd = oldcwd
	end
end

local function loadtarget(targetname)
	if targets[targetname] then
		return targets[targetname]
	end

	local target
	if not targetname:find(":") then
		target = {
			outs = {targetname},
			is = {
				__implicitfile = true
			}
		}
		targets[targetname] = target
	else
		local _, _, filepart, targetpart = targetname:find("^([^:]*):(%w+)$")
		if not filepart or not targetpart then
			error(string.format("malformed target name '%s'", targetname))
		end
		if (filepart == "") then
			filepart = cwd
		end
		local filename = concatpath(filepart, "/build.lua")
		loadbuildfile(concatpath(filename))

		target = targets[targetname]
		if not target then
			error(string.format("build file '%s' contains no rule '%s'",
				filename, targetpart))
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
			elseif s:find("^:") then
				s = cwd..s
			elseif s:find("^[^/]") then
				s = concatpath(cwd, s)
			end
			o[#o+1] = loadtarget(s)
		end
		return o
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
		return i
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
			if not e[propname] then
				if not typespec.optional then
					error(string.format("missing mandatory property '%s'", propname))
				end

				args[propname] = typespec.default
			else
				args[propname] = typeconverters[typespec.type](propname, e[propname])
				e[propname] = nil
			end
		end

		local propname, _ = next(e)
		if propname then
			error(string.format("don't know what to do with property '%s'", propname))
		end

		args.environment = environment

		local result = cb(args) or {}
		result.is = result.is or {}
		result.is[rulename] = true
		targets[cwd..":"..args.name] = result
		return result
	end
end

-----------------------------------------------------------------------------
--                              DEFAULT RULES                              --
-----------------------------------------------------------------------------

function environment:rule(ins, outs)
	local firstout = outs[1]
	for i = 2, #outs do
		emit(outs[i]..":", outs[1], "\n")
	end
	for i = 1, #ins do
		emit(firstout..":", ins[i], "\n")
	end
	emit(firstout..":\n")
end

function environment:label(...)
	local s = table.concat({...}, " ")
	emit("\t@echo", s, "\n")
end

function environment:mkdirs(dirs)
	dirs = uniquify(dirs)
	if (#dirs > 0) then
		emit("\t@mkdir -p", dirs, "\n")
	end
end

function environment:exec(commands)
	emit("\t$(hide)", table.concat(commands, " && "), "\n")
end

function environment:endrule()
	emit("\n")
end

definerule("simplerule",
	{
		ins = { type="targets" },
		outs = { type="strings" },
		label = { type="string", optional=true },
		commands = { type="strings" },
	},
	function (e)
		e.environment:rule(filenamesof(e.ins), e.outs)
		e.environment:label(cwd..":"..e.name, " ", e.label or "")
		e.environment:mkdirs(dirnames(filenamesof(e.outs)))
		e.environment:exec(
			templateexpand(e.commands,
				{
					ins = e.ins,
					outs = e.outs
				}
			)
		)
		e.environment:endrule()

		return {
			outs = e.outs
		}
	end
)

-----------------------------------------------------------------------------
--                               MAIN PROGRAM                              --
-----------------------------------------------------------------------------

globals = {
	asstring = asstring,
	concatpath = concatpath,
	cwd = cwd,
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

