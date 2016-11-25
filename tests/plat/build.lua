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
			"tests/plat/*.p"
		)

		local tests = {}
		for _, f in ipairs(testfiles) do
			local fs = replace(basename(f), "%..$", "")
			local _, _, lang = fs:find("_(.)$")
			if not lang then
				lang = "e"
			end

			local bin = ackprogram {
				name = fs.."_bin",
				srcs = { f },
				deps = { "tests/plat/lib+lib" },
				vars = {
					plat = e.plat,
					lang = lang,
					ackcflags = "-O0"
				}
			}

			tests[#tests+1] = normalrule {
				name = fs,
				outleaves = { "stamp" },
				ins = {
					bin,
					"tests/plat/testdriver.sh"
				},
				commands = {
					"%{ins[2]} "..e.method.." %{ins[1]} 5",
					"touch %{outs}"
				}
			}
		end

		return normalrule {
			name = e.name,
			outleaves = { "stamp" },
			ins = tests,
			commands = { "touch %{outs}" }
		}
	end
)