cprogram {
	name = "tabgen",
	srcs = { "./tabgen.c" }
}

definerule("tabgen",
	{
		srcs = { type="targets" },
	},
	function(e)
		local symname = replace(basename(e.srcs[1]), "%.tab$", "")

		return normalrule {
			name = e.name,
			ins = {
				"util/cmisc+tabgen",
				unpack(e.srcs)
			},
			outleaves = { symname..".c" },
			commands = {
				"%{ins[1]} -f%{ins[2]} > %{outs} || rm %{outs}"
			}
		}
	end
)
