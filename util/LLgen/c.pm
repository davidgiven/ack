-- This file is part of Prime Mover v0.1pre1.
-- (C) 2006 David Given
--
-- pm includefile to compile *host* C programs.

-- Standard Lua boilerplate.

local io_open = io.open
local string_gsub = string.gsub
local string_gfind = string.gfind
local table_insert = table.insert
local table_getn = table.getn
local filetime = pm.filetime

-- Define some variables.

CC = "gcc %CBUILDFLAGS% %CDYNINCLUDES% %CINCLUDES% %CEXTRAFLAGS% -c -o %out% %in%"
CPROGRAM = "gcc %CBUILDFLAGS% %CLINKFLAGS% %CEXTRAFLAGS% -o %out% %in% %CLIBRARIES%"
CDEPENDS = "gcc %CBUILDFLAGS% %CDYNINCLUDES% %CINCLUDES% %CEXTRAFLAGS% -MM -MG -MF %out% %in%"
AR = "%RM% %out% && ar cr %out% %in%"

CBUILDFLAGS = "-g -Os"
CINCLUDES = {}
CEXTRAFLAGS = ""
CLINKFLAGS = ""
CDYNINCLUDES = ""
CLIBRARIES = ""

--- Manage C file dependencies ----------------------------------------------

local dependency_cache = {}
local function load_dependency_file(fn)
	local o = dependency_cache[fn]
	if o then
		return o
	end
	
	-- Read in the dependency file.
	
	local f = io_open(fn)
	if not f then
		print("failed to open "..fn)
		return nil
	end
	f = f:read("*a")
	
	-- Massage the dependency file into a string containing one unescaped
	-- filename per line.
	
	f = string_gsub(f, "^.*[^\\]: *", "")
	f = string_gsub(f, "\\\r?\n", "")
	f = string_gsub(f, "([^\\]) +", "%1\n")
	f = string_gsub(f, "\\", "")
	
	-- Parse the string.
	
	o = {}
	for l in string_gfind(f, "[^\n]+") do
		table_insert(o, l)
	end
	
	dependency_cache[fn] = o
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
		local obj = simple {
			CDYNINCLUDES = self.CDYNINCLUDES,
			command = self.makedepends,
			outputs = {"%U%-%I%.d"},
			unpack(inputs)
		}
		local o = obj:__build()
		local depends = load_dependency_file(o[1])
		if not depends then
			self:__error("could not determine the dependencies for ",
				pm.rendertable(inputs))
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
