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
		for k, v in pairs(e.outleaves) do
			realouts[k] = concatpath(dir, v)
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

		local hdrs = filenamesof(e.hdrs)

		local commands = {}
		for _, s in ipairs(e.commands) do
			commands[#commands+1] = s
		end
		if (#hdrs > 0) then
			commands[#commands+1] = "cp %{hdrs} %{dir}"
		end

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			ins = ins,
			deps = concat(e.hdrs, e.deps),
			outleaves = { e.name..".a", unpack(basename(hdrs)) },
			label = e.label,
			commands = commands,
			vars = {
				hdrs = hdrs
			}
		}
	end
)

definerule("cprogram",
	{
		srcs = { type="targets", default={} },
		deps = { type="targets", default={} },
		commands = {
			type="strings",
			default={
				"$(CC) -o %{outs[1]} -Wl,--start-group %{ins} -Wl,--end-group"
			},
		}
	},
	function (e)
		local libs = filenamesof(e.deps, "%.a$")
		if (#e.srcs > 0) then
			for _, f in pairs(filenamesof(
				{
					clibrary {
						name = e.name .. "/main",
						cwd = e.cwd,
						srcs = e.srcs,
						deps = e.deps,
					}
				},
				"%.a$"
			)) do
				libs[#libs+1] = f
			end
		end

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			deps = e.deps,
			ins = libs,
			outleaves = { e.name },
			commands = e.commands,
		}
	end
)

