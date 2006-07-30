-- pm includefile to compile *host* C programs.

-- Standard Lua boilerplate.

local io_open = io.open
local string_gsub = string.gsub
local string_gfind = string.gfind
local string_find = string.find
local table_insert = table.insert
local table_getn = table.getn
local filetime = pm.filetime

-- Define some variables.

CCOMPILER = "gcc"
CC = "%CCOMPILER% %CBUILDFLAGS% %CDYNINCLUDES% %CINCLUDES% %CDEFINES% %CEXTRAFLAGS% -c -o %out% %in%"
CPROGRAM = "%CCOMPILER% %CBUILDFLAGS% %CLINKFLAGS% %CEXTRAFLAGS% -o %out% %in% %CLIBRARIES%"
AR = "%RM% %out% && ar cr %out% %in% && ranlib %out%"

CBUILDFLAGS = {"-g"}
CINCLUDES = EMPTY
CDEFINES = EMPTY
CEXTRAFLAGS = EMPTY
CLINKFLAGS = EMPTY
CDYNINCLUDES = EMPTY
CLIBRARIES = EMPTY

--- Manage C file dependencies ----------------------------------------------

local dependency_cache = {}
local function calculate_dependencies(filename, includes)
	-- Cache values, so we don't recalculate dependencies needlessly.
	
	local o = dependency_cache[filename]
	if o then
		return o
	end
	
	local deps = {}
	deps[filename] = true
	
	local calcdeps = 0
	calcdeps = function(filename, file)
		file = file or io.open(filename)
		if not file then
			return
		end
		
		local localincludes = string_gsub(filename, "/[^/]*$", "")
		if localincludes then
			localincludes = {localincludes, unpack(includes)}
		else
			localincludes = includes
		end
			
		for line in file:lines() do
			local _, _, f = string_find(line, '^[ \t]*#[ \t]*include[ \t]*["<]([^"]+)[">]')
			if f then
				for _, path in ipairs(localincludes) do
					local subfilename = path.."/"..f
					local subfile = io.open(subfilename)
					if subfile then
						if not deps[subfilename] then
							deps[subfilename] = true
							calcdeps(subfilename, subfile)
						end
						break
					end
				end
			end
		end
		
		-- Explicit close to avoid having to wait for the garbage collector
		-- to free up the underlying fd.
		
		file:close()
	end
	
	calcdeps(filename)
	o = {}
	for i, _ in pairs(deps) do
		table_insert(o, i)
	end
		
	dependency_cache[filename] = o
	return o
end

-- This clause specialises 'simple' to add support for smart dependencies of C
-- files.

simple_with_clike_dependencies = simple {
	class = "simple_with_clike_dependencies",
	makedepends = {"%CDEPENDS%"},

	__init = function(self, p)
 		simple.__init(self, p)
  		
		-- If we're a class, don't verify.
		
		if ((type(p) == "table") and p.class) then
			return
		end

		-- If dynamicheaders is an object, turn it into a singleton list.
		
		if self.dynamicheaders then
			if (type(self.dynamicheaders) ~= "table") then
				self:__error("doesn't know what to do with dynamicheaders, which ",
					"should be a list or an object but was a ", type(self.dynamicheaders))
			end
			if self.dynamicheaders.class then
				self.dynamicheaders = {self.dynamicheaders}
			end
		end
	end,
	
	__dependencies = function(self, inputs, outputs)		
		local cincludes = self.CINCLUDES
		if (type(cincludes) == "string") then
			cincludes = {cincludes}
		end
		
		local includes = {}
		for _, i in ipairs(cincludes) do
			local _, _, p = string_find(i, "^-I[ \t]*(.+)$")
			if p then
				table_insert(includes, p)
			end
		end
		
		local depends = calculate_dependencies(inputs[1], includes)
		if not depends then
			self:__error("could not determine the dependencies for ",
				pm.rendertable(inputs))
		end
		if pm.verbose then
			pm.message('"', inputs[1], '" appears to depend on ',
				pm.rendertable(depends))
		end
		return depends
	end,
	
	__buildadditionalchildren = function(self)
		self.CDYNINCLUDES = ""
		if self.dynamicheaders then
			for _, i in ipairs(self.dynamicheaders) do
				local o = i:__build()
				if o[1] then
					self.CDYNINCLUDES = self.CDYNINCLUDES..' "-I'..string_gsub(o[1], "/[^/]*$", "")..'"'
				end
			end
		end
	end
}

-- These are the publically useful clauses.

cfile = simple_with_clike_dependencies {
	class = "cfile",
	command = {"%CC%"},
	outputs = {"%U%-%I%.o"},
}

cprogram = simple {
	class = "cprogram",
	command = {"%CPROGRAM%"},
	outputs = {"%U%-%I%"},
}

clibrary = simple {
	class = "clibrary",
	command = {
		"%AR%"
	},
	outputs = {"%U%-%I%.a"},
}
