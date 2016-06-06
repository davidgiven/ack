simplerule {
	name = "random",
	ins = {},
	outs = {"out"},
	commands = {
		"dd if=/dev/random of=%{outs} bs=1024 count=1"
	}
}

simplerule {
	name = "sorted",
	ins = { ":random" },
	outs = { "sorted" },
	commands = {
		"sort %{ins} > %{outs}"
	}
}

--[[
function environment:cfileflags()
	emit("$CFLAGS")
end

function environment:cfile(srcs, obj, includes)
	emit("$CC -o", obj, srcs)
	emit(ab.expand(includes, "-I%"))
	self:cflags()
	emit("\n")
end

function environment:clinkflags()
	emit("$LDFLAGS")
end

function environment:clink(objs, exe, libraryFlags)
	emit("$CC -o", exe, objs, libraryFlags)
	self:clinkflags()
	emit("\n")
end

--
-- Targets:
--
-- {
--   dir = target's build directory
--   outs = target's object files
--   is = { set of rule types which made the target }
-- }
function M.subenv(p)
	local e = p[1]
	setmetable(p, {__index = e})
	return p
end

local function check_filename(fn)
	if type(fn) == "table" then
		for _, f in ipairs(fn) do
			check_filename(f)
		end
	else
		if fn:find("%s") then
			error("Filename '"+fn+"' contains spaces. This will make the build system sad.")
		end
	end
end

function M.basename(fn)
	if type(fn) == "table" then
		local nfn = {}
		for _, f in ipairs(fn) do
			nfn[#nfn+1] = M.basename(f)
		end
		return nfn
	else
		local _, _, base = fn:find("([^/]*)$")
		return base
	end
end
		
function M.flatten(t)
	if t == nil then
		return {}
	end

	local tt = {}
	for _, subt in ipairs(t) do
		if type(subt) == "table" then
			for _, subt in ipairs(M.flatten(subt)) do
				tt[#tt+1] = subt
			end
		else
			tt[#tt+1] = subt
		end
	end
	return tt
end

local function append(...)
	local ts = {}
	for _, t in ipairs({...}) do
		for _, v in ipairs(t) do
			ts[#ts+1] = v
		end
	end
	return ts
end

local function settotable(s)
	local t = {}
	for k in pairs(s) do
		t[#t+1] = k
	end
	return t
end

local function asstring(t)
	return table.concat(M.flatten(t), " ")
end

local function emit(...)
	for _, s in ipairs({...}) do
		io.stdout:write(s)
	end
end

function M.rawtarget(p)
	local description = p.description or error("no description supplied")
	local ins = M.flatten(p.ins)
	local outs = M.flatten(p.outs)

	local cmd = p.command
	if type(cmd) ~= "table" then
		cmd = {cmd}
	end

	for _, s in ipairs(ins) do
		check_filename(s)
	end
	for _, s in ipairs(outs) do
		check_filename(s)
	end

	emit(outs[1], ":")
	for _, s in ipairs(ins) do
		emit(" ", s)
	end
	emit("\n")

	emit("\t@echo ", p.description, "\n")

	emit("\t$(hide) ", table.concat(cmd, " && "), "\n")

	for i = 2, #outs do
		emit(outs[i], ": ", outs[1], "\n")
	end

	emit("\n")
end

function M.export(p)
	local e = p[1]
	local dest = p.dest or error("no export destination provided")
	local deps = p.deps or error("nothing to export")

	local fdeps = M.flatten(deps)

	if #fdeps ~= 1 then
		error("you can only export one thing at a time")
	end

	return M.rawtarget {e,
		ins=deps,
		outs={dest},
		command="cp "..fdeps[1].." "..dest,
		description="EXPORT "..dest
	}
end

function M.hermetic(p)
	local e = p[1]
	local ins = M.flatten(p.ins)
	local deps = M.flatten(p.deps)
	local baseouts = p.baseouts or error("you must specify some baseouts")
	local description = p.description

	local absouts = {}
	local path = e.PATH .. "/" .. p.baseouts[1] .. ".env"

	for _, s in ipairs(M.flatten(p.baseouts)) do
		absouts[#absouts+1] = path .. "/" .. s
	end

	local dirset = {}
	for _, s in ipairs(absouts) do
		local d = s:gsub("^(.*/).*$", "%1")
		if d then
			dirset[d] = true
		end
	end

	local newcmd = {
		"rm -rf "..path,
		"mkdir -p "..asstring(settotable(dirset)),
		"ln -srf "..asstring(append(ins, deps)).." "..path,
		"cd "..path
	}
	for _, s in ipairs(p.command) do
		newcmd[#newcmd+1] = "(" .. s .. ")"
	end

	M.rawtarget {e,
		ins={ins, unpack(deps)},
		outs=absouts,
		command=newcmd,
		description=description
	}

	return absouts
end

function M.cfile(p)
	local e = p[1]
	local src = p.src
	local deps = p.deps or {}

	local outfile = p.src:gsub("%.c$", ".o")
	local basesrc = M.basename(p.src)

	return M.hermetic {e,
		ins={src},
		deps=deps,
		baseouts={outfile},
		command={e.CC.." "..e.CFLAGS.." -c -o "..outfile.." "..basesrc},
		description="CC "..src
	}
end

function M.cprogram(p)
	local e = p[1]
	local name = p.name or error("cprogram must have a name specified")
	local deps = p.deps or {}
	local libs = p.libraries or {}
	local headers = p.headers or {}

	if p.srcs then
		local mainlib = M.clibrary {e,
			name=name..".a",
			deps=deps,
			srcs=p.srcs,
			headers=headers
		}

		deps = append(deps, {mainlib})
	end

	local libflags = {}
	for _, s in ipairs(libs) do
		libflags[#libflags+1] = "-l"..s
	end

	return M.hermetic {e,
		ins=deps,
		baseouts={name},
		command={e.CC.." "..e.CFLAGS.." -o "..name.." "..asstring(M.basename(deps)).." "..asstring(libflags)},
		description="CLINK "..name
	}
end

function M.clibrary(p)
	local e = p[1]
	local name = p.name or error("clibrary must have a name specified")
	local deps = M.flatten(p.deps)
	local srcs = M.flatten(p.srcs)
	local headers = M.flatten(p.headers)

	local baseouts = {name}

	local objs = deps
	for _, f in ipairs(srcs) do
		objs[#objs+1] = M.cfile {e,
			src=f,
			deps=headers
		}
	end

	return M.hermetic {e,
		ins=append(objs, headers),
		baseouts=baseouts,
		command={
			e.AR.." q "..name.." "..asstring(M.basename(objs)),
			e.RANLIB.." "..name,
		},
		description="CLIBRARY "..name
	}
end

emit("hide = @\n")

return M
--]]
