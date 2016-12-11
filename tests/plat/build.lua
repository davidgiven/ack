include("plat/build.lua")

definerule("plat_testsuite",
	{
		plat = { type="string" },
		method = { type="string" },
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there.
		local testfiles = filenamesof(
			"tests/plat/*.c",
			"tests/plat/*.e",
			"tests/plat/*.p",
			"tests/plat/bugs/*.mod"
		)

		acklibrary {
			name = "lib",
			srcs = { "tests/plat/lib/test.c" },
			hdrs = {
				"tests/plat/lib/test.h",
				"tests/plat/lib/Test.def"
			},
			vars = { plat = e.plat },
		}

		local tests = {}
		for _, f in ipairs(testfiles) do
			local fs = replace(basename(f), "%.[^.]+$", "")
			local _, _, lang = fs:find("_([^_]+)$")
			if not lang then
				lang = "e"
			end

			local bin = ackprogram {
				name = fs.."_bin",
				srcs = { f },
				deps = { "+lib" },
				vars = {
					plat = e.plat,
					lang = lang,
					ackcflags = "-O0"
				}
			}

			tests[#tests+1] = normalrule {
				name = fs,
				outleaves = { e.plat.."-"..fs.."-testlog.txt" },
				ins = {
					bin,
					"tests/plat/testdriver.sh",
					"util/build+testrunner"
				},
				commands = {
					"(%{ins[2]} "..e.method.." %{ins[1]} 5 %{ins[3]} || echo FAILED) > %{outs}",
				}
			}
		end

		return bundle {
			name = e.name,
			srcs = tests,
		}
	end
)