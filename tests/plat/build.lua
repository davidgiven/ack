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
			-- added structcopy_e.c
			"tests/plat/*.c",
			"tests/plat/*.e",
			"tests/plat/*.p",
			"tests/plat/b/*.b",
			"tests/plat/bugs/*.c",
			"tests/plat/bugs/*.mod",
			"tests/plat/m2/*.mod"
		)

		acklibrary {
			name = "lib",
			srcs = {
				"tests/plat/lib/test.c",
				"tests/plat/lib/test_b.c",
			},
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
					ackcflags = "-O0 -Bmain"
				}
			}

			local methoddep = nil
			local methodpath = e.method
			if e.method:find("%+") then
				methoddep = e.method
				methodpath = "%{ins[4]}"
			end
			tests[#tests+1] = normalrule {
				name = fs,
				outleaves = { e.plat.."-"..fs.."-testlog.txt" },
				ins = {
					bin,
					"tests/plat/testdriver.sh",
					"util/build+testrunner",
					methoddep,
				},
				commands = {
					"%{ins[2]} "..methodpath.." %{ins[1]} 15 %{ins[3]} > %{outs}; true",
				}
			}
		end

		return bundle {
			name = e.name,
			srcs = tests,
		}
	end
)
