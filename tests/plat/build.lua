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
			"tests/plat/dup_e.e",
			"tests/plat/exg_e.e",
			"tests/plat/inn_e.e",
			"tests/plat/rck_e.e",
			"tests/plat/rotate_e.e",
			"tests/plat/*.p",
			"tests/plat/b/*.b",
			"tests/plat/bugs/bug-22-inn_mod.mod",
			"tests/plat/bugs/bug-62-notvar_var_e.c",
			"tests/plat/m2/ConvTest_mod.mod",
			"tests/plat/m2/NestProc_mod.mod",
			"tests/plat/m2/OpenArray_mod.mod",
			"tests/plat/m2/Set100_mod.mod",
			"tests/plat/m2/StringTest_mod.mod"
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

			tests[#tests+1] = normalrule {
				name = fs,
				outleaves = { e.plat.."-"..fs.."-testlog.txt" },
				ins = {
					bin,
					"tests/plat/testdriver.sh",
					"util/build+testrunner"
				},
				commands = {
					"%{ins[2]} "..e.method.." %{ins[1]} 15 %{ins[3]} > %{outs}; true",
				}
			}
		end

		return bundle {
			name = e.name,
			srcs = tests,
		}
	end
)
