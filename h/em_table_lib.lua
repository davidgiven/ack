local function strip(s)
	return s:match("^%s*(.-)%s*$")
end

function load_table()
	local specs = {}
	for line in io.lines() do
		line = strip(line)
		if line == "" then
			break
		end

		local _, _, name, num = line:find("^(%w+)%s+(%d+)$")
		if not name then
			error("malformed spec line in em_table")
		end

		specs[#specs+1] = {
			name = name,
			num = tonumber(num)
		}
	end

	for _, v in ipairs(specs) do
		specs[v.name] = v
	end

	local pseudos = {}
	local pseudo_count = 0
	for line in io.lines() do
		line = strip(line)
		if line == "" then
			break
		end

		local _, _, name, num, flags = line:find("^(%w+)%s+(%d+)%s+([^ \t]+)$")
		if not name then
			error("malformed pseudo line in em_table: ")
		end

		pseudos[#pseudos+1] = {
			name = name,
			num = tonumber(num) + specs.fpseu.num,
			flags = flags
		}
		pseudo_count = pseudo_count + 1
	end

	local mnems = {}
	local mnem_count = 0
	for line in io.lines() do
		line = strip(line)
		if line == "" then
			break
		end

		local _, _, name, flags1, flags2 = line:find("^(%w+)%s+([^ \t]+)%s+([^ \t]+)$")
		if not name then
			error("malformed mnemonic line in em_table")
		end

		mnem_count = mnem_count + 1
		mnems[#mnems+1] = {
			name = name,
			num = mnem_count,
			flags1 = flags1,
			flags2 = flags2
		}
	end

	local lpseu = {
		name = "lpseu",
		num = specs.fpseu.num + pseudo_count - 1
	}
	local lmnem = {
		name = "lmnem",
		num = specs.fmnem.num + mnem_count - 1
	}

	specs[#specs+1] = lpseu
	specs.lpseu = lpseu

	specs[#specs+1] = lmnem
	specs.lmnem = lmnem

	return specs, pseudos, mnems
end

