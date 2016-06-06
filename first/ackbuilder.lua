-- Targets:
--
-- {
--   dir = target's build directory
--   outs = target's object files
--   is = { set of rule types which made the target }
-- }

local environment = {}
local rules = {}

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

local function definerule(name, cb)
	if rules[name] then
		error(string.format("rule '%s' is already defined", name))
	end
end

-----------------------------------------------------------------------------
--                               MAIN PROGRAM                              --
-----------------------------------------------------------------------------

local globals = {
	asstring = asstring,
	definerule = definerule,
	emit = emit,
	environment = environment,
}
setmetatable(globals, {__index = _G})

emit("hide=@\n")
for _, file in ipairs({...}) do
	local data, chunk, e
	data = io.open(file):read("*a")
	if not e then
		local thisglobals = {_G = thisglobals}
		setmetatable(thisglobals, {__index = globals})
		chunk, e = loadstring(data, file, "text", thisglobals)
	end
	if e then
		error(string.format("couldn't load '%s': %s", file, e))
	end

	local _, e = pcall(chunk)
end

