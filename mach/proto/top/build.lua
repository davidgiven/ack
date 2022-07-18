include("util/topgen/build.lua")

definerule("build_top",
	{
		arch = { type="string" },
	},
	function(e)
		local t = topgen {
			name = e.name.."_topgen",
			srcs = { "mach/"..e.arch.."/top/table" }
		}

		return cprogram {
			name = e.name,
			srcs = {
				"mach/proto/top/queue.c",
				"mach/proto/top/top.c",
			},
			deps = { t },
		}
	end
)
