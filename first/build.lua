definerule("normalrule",
	{
		ins = { type="targets" },
		outleaves = { type="strings" },
		label = { type="string", optional=true },
		commands = { type="strings" },
		vars = { type="table", default={} },
	},
	function (e)
		local objpath = "$(OBJDIR)/"..e.name
		local realouts = {}
		for k, v in pairs(e.outleaves) do
			realouts[k] = concatpath(objpath, v)
		end

		local vars = inherit(e.vars, {
			dir = objpath
		})

		local result = simplerule {
			name = e.name,
			ins = e.ins,
			outs = realouts,
			label = e.label,
			commands = e.commands,
			vars = vars,
		}
		result.dir = objpath
		return result
	end
)

definerule("cfile",
	{
		srcs = { type="targets" },
		deps = { type="targets", default={} },
		commands = {
			type="strings",
			default={
				"$(CC) -c -o %{outs[1]} %{ins[1]} %{hdrpaths}"
			},
		}
	},
	function (e)
		local csrcs = filenamesof(e.srcs, "%.c$")
		if (#csrcs ~= 1) then
			error("you must have exactly one .c file")
		end
		
		local hsrcs = filenamesof(e.srcs, "%.h$")
		local hdeps = selectof(e.deps, "%.h$")
		local hdrpaths = {}
		for _, t in pairs(hdeps) do
			hdrpaths[#hdrpaths+1] = "-I"..t.dir
		end
		hdrpaths = uniquify(hdrpaths)

		for _, f in pairs(filenamesof(hdeps)) do
			hsrcs[#hsrcs+1] = f
		end

		local outleaf = basename(csrcs[1]):gsub("%.c$", ".o")

		return normalrule {
			name = e.name,
			ins = {csrcs[1], unpack(hsrcs)},
			outleaves = {outleaf},
			label = e.label,
			commands = e.commands,
			vars = {
				hdrpaths = hdrpaths
			}
		}
	end
)

definerule("bundle",
	{
		srcs = { type="targets" },
		commands = {
			type="strings",
			default={
				"tar cf - %{ins} | (cd %{dir} && tar xf -)"
			}
		}
	},
	function (e)
		local outleaves = {}
		local commands = {}
		for _, f in pairs(filenamesof(e.srcs)) do
			local localf = basename(f)
			outleaves[#outleaves+1] = localf
			commands[#commands+1] = "cp "..f.." %{dir}/"..localf
		end

		return normalrule {
			name = e.name,
			ins = e.srcs,
			outleaves = outleaves,
			label = e.label,
			commands = commands
		}
	end
)

definerule("clibrary",
	{
		srcs = { type="targets" },
		deps = { type="targets", default={} },
		commands = {
			type="strings",
			default={
				"rm -f %{outs}",
				"$(AR) qs %{outs} %{ins}"
			},
		}
	},
	function (e)
		local csrcs = filenamesof(e.srcs, "%.c$")
		if (#csrcs < 1) then
			error("you must supply at least one C source file")
		end

		local hsrcs = filenamesof(e.srcs, "%.h$")

		local ins = {}
		for _, csrc in pairs(csrcs) do
			local n = basename(csrc):gsub("%.%w*$", "")
			ins[#ins+1] = cfile {
				name = e.name.."/"..n,
				srcs = {csrc, unpack(hsrcs)},
				deps = e.deps,
			}
		end

		return normalrule {
			name = e.name,
			ins = ins,
			outleaves = { e.name..".a" },
			label = e.label,
			commands = e.commands
		}
	end
)

clibrary {
	name = "mylib",
	srcs = {
		"modules/src/string/*.c",
	},
	deps = {
		"modules:headers"
	}
}

--[[
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
