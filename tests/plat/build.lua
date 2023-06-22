
include("plat/build.lua")

definerule("plat_testsuite",
	{
		plat = { type="string" },
		method = { type="string" },
		-- added bugs/bug-203-ego-sr_c-O3.c
		sets = { type="table", default={"core", "b", "c", "m2", "pc", "floats", "long-long"}},
		skipsets = { type="table", default={}},
		tests = { type="targets", default={} },
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there.
		local testfiles = filenamesof(e.tests)
		local skipsets = {}
		for _, set in ipairs(e.skipsets) do
			skipsets[set] = true
		end
		for _, set in ipairs(e.sets) do
			if not skipsets[set] then
				local fs = filenamesof("tests/plat/"..set.."+srcs")
				for _, f in ipairs(fs) do
					testfiles[#testfiles+1] = f
				end
			end
		end

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

			-- If lang is "c-O3", then build with -O3.
			local _, _, optimize = lang:find("(-O[^-]*)$")
			if optimize then
				lang = lang:sub(1, -1 - #optimize)
			else
				optimize = "-O0"
			end

			local bin = ackprogram {
				name = fs.."_bin",
				srcs = { f },
				deps = { "+lib" },
				vars = {
					plat = e.plat,
					lang = lang,
					ackcflags = optimize.." -Bmain"
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
