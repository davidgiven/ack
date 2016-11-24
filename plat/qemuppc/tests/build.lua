include("plat/build.lua")

local qemu = "qemu-system-ppc"
local tests = {}

if os.execute("which "..qemu.." > /dev/null") ~= 0 then
	print("warning: skipping tests which require ", qemu)
else
	local testcases = filenamesof("./*.c", "./*.s", "./*.e", "./*.p")

	for _, f in ipairs(testcases) do
		local fs = replace(basename(f), "%..$", "")
		local _, _, lang = fs:find("_(.)$")
		if not lang then
			lang = "e"
		end

		local bin = ackprogram {
			name = fs.."_bin",
			srcs = { f },
			deps = { "plat/qemuppc/tests/lib+lib" },
			vars = {
				plat = "qemuppc",
				lang = lang,
				ackcflags = "-O0"
			}
		}

		tests[#tests+1] = normalrule {
			name = fs,
			outleaves = { "stamp" },
			ins = {
				bin,
				"./testdriver.sh"
			},
			commands = {
				"%{ins[2]} "..qemu.." %{ins[1]} 5",
				"touch %{outs}"
			}
		}
	end
end

normalrule {
	name = "tests",
	outleaves = { "stamp" },
	ins = tests,
	commands = { "touch %{outs}" }
}