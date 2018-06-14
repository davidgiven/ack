local function decode(line)
	local _, _, bits = line:find("^([^ ]+) ")
	if #bits ~= 32 then
		error("'"..bits.."' isn't 32 bits long")
	end

	local fields = {}
	local i = 1
	while i ~= 33 do
		local c = line:sub(i, i)
		if c ~= "." then
			local f = { pos=i }
			if c:find("%w") then
				f.size = 1
				f.value = c
			elseif c == "<" then
				local _, newi, name = line:find("^<%-*(%w+)%-*>", i)
				f.size = 1 + newi - i
				f.value = name
				i = newi
			else
				error("bad field char '"..c.."' in '"..line.."'")
			end
			if f.value:find("[0-9]+") then
				f.literal = true
				f.variable = false
			else
				f.literal = false
				f.variable = true
			end
			-- Convert from PowerPC numbering to sane numbering
			f.pos = 33-(f.pos + f.size)
			fields[#fields+1] = f
		end
		i = i + 1
	end
	return fields
end

local function emit(fields, code)
	local mask = 0
	local value = 0
	for _, f in ipairs(fields) do
		if f.literal then
			local s = math.pow(2, f.pos)
			local m = math.pow(2, f.size) - 1
			mask = mask + m*s
			value = value + f.value*s
		end
	end

	print(string.format("if ((value & 0x%x) == 0x%x) {", mask, value))
	for _, f in ipairs(fields) do
		if f.variable then
			local m = math.pow(2, f.size) - 1
			print(string.format("uint32_t %s = (value >> %d) & 0x%x;", f.value, f.pos, m))
		end
	end

	print(code)
	print("return;")
	print("}")
end

while true do
	local line = io.stdin:read("*l")
	if not line then
		break
	end
	line = line:gsub("#.*$", "")
	line = line:gsub(" *$", "")
	if line ~= "" then
		local fields = decode(line)
		emit(fields, line:sub(34, #line))
	end
end

