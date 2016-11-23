local function objdir(e)
	return concatpath("$(OBJDIR)", e.cwd, e.name)
end

definerule("normalrule",
	{
		ins = { type="targets" },
		deps = { type="targets", default={} },
		outleaves = { type="strings" },
		label = { type="string", optional=true },
		objdir = { type="string", optional=true },
		commands = { type="strings" },
	},
	function (e)
		local dir = e.objdir or objdir(e)
		local realouts = {}
		for _, v in pairs(e.outleaves) do
			realouts[#realouts+1] = concatpath(dir, v)
		end

		local vars = inherit(e.vars, {
			dir = dir
		})

		local result = simplerule {
			name = e.name,
			ins = e.ins,
			deps = e.deps,
			outs = realouts,
			label = e.label,
			commands = e.commands,
			vars = vars,
		}
		result.dir = dir
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
				"$(CC) -c -o %{outs[1]} %{ins[1]} %{hdrpaths} %{cflags}"
			},
		}
	},
	function (e)
		local hdrpaths = {}
		for _, t in pairs(e.deps) do
			if t.dir then
				hdrpaths[#hdrpaths+1] = "-I"..t.dir
			end
		end
		hdrpaths = uniquify(hdrpaths)

		local outleaf = basename(e.name)..".o"

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			ins = e.srcs,
			deps = e.deps,
			outleaves = {outleaf},
			label = e.label,
			commands = e.commands,
			vars = {
				hdrpaths = hdrpaths,
			}
		}
	end
)

definerule("cppfile",
	{
		srcs = { type="targets" },
		deps = { type="targets", default={} },
		outleaf = { type="string" },
		commands = {
			type="strings",
			default={
				"$(CC) -E -P -o %{outs[1]} %{hdrpaths} %{cflags} -x c %{ins}"
			}
		},
	},
	function (e)
		if (#e.srcs ~= 1) then
			error("you must have exactly one input file")
		end

		local hdrpaths = {}
		for _, t in pairs(e.deps) do
			hdrpaths[#hdrpaths+1] = "-I"..t.dir
		end
		hdrpaths = uniquify(hdrpaths)

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			ins = e.srcs,
			deps = e.deps,
			outleaves = {e.outleaf},
			label = e.label,
			commands = e.commands,
			vars = {
				hdrpaths = hdrpaths,
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
		for _, f in fpairs(e.srcs) do
			local localf = basename(f)
			outleaves[#outleaves+1] = localf
			commands[#commands+1] = "cp "..f.." %{dir}/"..localf
		end

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			ins = e.srcs,
			outleaves = outleaves,
			label = e.label,
			commands = commands
		}
	end
)

definerule("clibrary",
	{
		srcs = { type="targets", default={} },
		hdrs = { type="targets", default={} },
		deps = { type="targets", default={} },
		_cfile = { type="object", default=cfile },
		commands = {
			type="strings",
			default={
				"rm -f %{outs[1]}",
				"$(AR) cqs %{outs[1]} %{ins}",
			},
		}
	},
	function (e)
		local ins = {}
		for _, src in fpairs(e.srcs) do
			local n = basename(src):gsub("%.%w*$", "")
			ins[#ins+1] = e._cfile {
				name = e.name.."/"..n,
				cwd = e.cwd,
				srcs = {src},
				deps = e.deps,
				vars = {
					["+cflags"] = { "-I"..e.cwd, },
				},
			}
		end

		local commands = {}
		local outleaves = {}
		if (#e.srcs > 0) then
			for _, s in ipairs(e.commands) do
				commands[#commands+1] = s
			end
			outleaves[#outleaves+1] = e.name..".a"
		end

		local hdrsin = {}
		for dest, src in pairs(e.hdrs) do
			if (type(dest) == "number") then
				for _, f in ipairs(filenamesof(src)) do
					hdrsin[#hdrsin+1] = f
					outleaves[#outleaves+1] = basename(f)
					commands[#commands+1] = "cp "..asstring(f).." %{dir}"
				end
			else
				local fs = filenamesof(src)
				if (#fs ~= 1) then
					error(string.format("keyed header '%s' can only be a single file", dest))
				end
				local f = fs[1]

				hdrsin[#hdrsin+1] = f
				outleaves[#outleaves+1] = dest
				commands[#commands+1] = "cp "..asstring(f).." %{dir}/"..dest
			end
		end

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			ins = ins,
			deps = {hdrsin, e.deps},
			outleaves = outleaves,
			label = e.label,
			commands = commands,
		}
	end
)

definerule("cprogram",
	{
		srcs = { type="targets", default={} },
		deps = { type="targets", default={} },
		_clibrary = { type="object", default=clibrary },
		commands = {
			type="strings",
			default={
				"$(CC) -o %{outs[1]} -Wl,--start-group %{ins} -Wl,--end-group"
			},
		}
	},
	function (e)
		local libs = matching(filenamesof(e.deps), "%.a$")
		local srcs = {}
		if (#e.srcs > 0) then
			for _, f in pairs(
				matching(
					filenamesof(
						e._clibrary {
							name = e.name .. "/main",
							cwd = e.cwd,
							srcs = e.srcs,
							deps = e.deps,
						}
					),
					"%.a$"
				)
			) do
				srcs[#srcs+1] = f
			end
		end

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			deps = e.deps,
			ins = { srcs, libs },
			outleaves = { e.name },
			commands = e.commands,
		}
	end
)

