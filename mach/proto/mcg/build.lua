include("util/mcgg/build.lua")

definerule("build_mcg",
	{
		arch = { type="string" }
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		local headers = clibrary {
			name = e.name.."/headers",
			srcs = {},
			hdrs = {
				"mach/proto/mcg/basicblock.h",
				"mach/proto/mcg/graph.h",
				"mach/proto/mcg/hop.h",
				"mach/proto/mcg/ir.h",
				"mach/proto/mcg/mcgg_generated_footer.h",
				"mach/proto/mcg/mcgg_generated_header.h",
				"mach/proto/mcg/mcg.h",
				"mach/proto/mcg/procedure.h",
				"mach/proto/mcg/reg.h",
				"mach/"..e.arch.."/mcg+headers",
			}
		}

		local tables = mcgg {
			name = e.name.."/tables",
			srcs = { "mach/"..e.arch.."/mcg/table" }
		}

		return cprogram {
			name = e.name,
			srcs = {
				"mach/proto/mcg/basicblock.c",
				"mach/proto/mcg/data.c",
				"mach/proto/mcg/graph.c",
				"mach/proto/mcg/hop.c",
				"mach/proto/mcg/ir.c",
				"mach/proto/mcg/main.c",
				"mach/proto/mcg/parse_em.c",
				"mach/proto/mcg/pass_convertstackops.c",
				"mach/proto/mcg/pass_eliminatetrivialblocks.c",
				"mach/proto/mcg/pass_groupirs.c",
				"mach/proto/mcg/pass_instructionselection.c",
				"mach/proto/mcg/pass_livevreganalysis.c",
				"mach/proto/mcg/pass_lowerpushes.c",
				"mach/proto/mcg/pass_phigroups.c",
				"mach/proto/mcg/pass_prologueepilogue.c",
				"mach/proto/mcg/pass_registerallocator.c",
				"mach/proto/mcg/pass_removedeadblocks.c",
				"mach/proto/mcg/pass_removedeadphis.c",
				"mach/proto/mcg/pass_returnvalues.c",
				"mach/proto/mcg/pass_splitcriticaledges.c",
				"mach/proto/mcg/pass_ssa.c",
				"mach/proto/mcg/pass_typeinference.c",
				"mach/proto/mcg/predicates.c",
				"mach/proto/mcg/procedure.c",
				"mach/proto/mcg/reg.c",
				"mach/proto/mcg/symbol.c",
				"mach/proto/mcg/treebuilder.c",
				"mach/"..e.arch.."/mcg/platform.c",
				matching(filenamesof(tables), "%.c$")
			},
			deps = {
				"h+emheaders",
				"modules+headers",
				"modules/src/alloc+lib",
				"modules/src/data+lib",
				"modules/src/em_code+lib_k",
				"modules/src/em_data+lib",
				"modules/src/idf+lib",
				"modules/src/object+lib",
				"modules/src/read_em+lib_kv",
				"modules/src/string+lib",
				"modules/src/system+lib",
				"util/mcgg+lib",
				headers,
				tables, -- for .h file
			}
		}
	end
)

