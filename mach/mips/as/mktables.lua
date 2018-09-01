local args = {...}

local wordscount = 0
local words = {}
local insns = {}

local function addword(word)
	local n = words[word]
	if not n then
		words[word] = wordscount
		wordscount = wordscount + 1
		n = wordscount
	end
	return n
end

local function parsesyntax(line)
    local syntax = {}
    for word in line:gmatch("%S+") do
		local _, _, s = word:find('^"(.*)"$')
        if s then
			syntax[#syntax+1] = {word=addword(s) }
		else
            _, _, s = word:find("^('.*')$")
            if s then
                syntax[#syntax+1] = {punct=s }
			else
				local token = {}
				for equate in word:gmatch("([^=]+)=") do
					token[#token+1] = equate
                end
				_, _, token.token = word:find("([^=]*)$")
                syntax[#syntax+1] = token
                if not token.token then
					error("can't parse "..word)
				end
			end
		end
	end
	return syntax
end

local function parsefields(line)
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
            if f.value then
				fields[f.value] = f
			end
		end
		i = i + 1
    end

	local value = 0
	for _, f in ipairs(fields) do
		if f.literal then
			local s = math.pow(2, f.pos)
			value = value + f.value*s
		end
    end
    fields.value = value

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
		local fields = parsefields(line)
		local syntax = parsesyntax(line:sub(34, #line))
    	insns[#insns+1] = {
			fields=parsefields(line),
			syntax=parsesyntax(line:sub(34, #line))
		}
	end
end

local definitionsfp = io.open(args[1], "w")
for word, value in pairs(words) do
	definitionsfp:write("%token <y_word> OP_", tostring(value), " /* ", word, " */\n")
end
definitionsfp:close()

local tokensfp = io.open(args[2], "w")
for word, value in pairs(words) do
	tokensfp:write("0, OP_", tostring(value), ", 0, \"", word, "\",\n")
end
tokensfp:close()

local rulesfp = io.open(args[3], "w")
rulesfp:write("operation\n")
for index, insn in ipairs(insns) do
	if index == 1 then
		rulesfp:write("\t:")
	else
		rulesfp:write("\t|")
	end
	for _, word in ipairs(insn.syntax) do
		if word.word then
			rulesfp:write(" OP_", tostring(word.word))
		end
		if word.punct then
			rulesfp:write(" ", word.punct)
		end
		if word.token then
			rulesfp:write(" ", word.token)
		end
	end
	rulesfp:write("\n")

	rulesfp:write("\t{ emit4(", string.format("0x%08x", insn.fields.value))
	for wordindex, word in ipairs(insn.syntax) do
		if word.token then
			for _, alias in ipairs(word) do
				local f = insn.fields[alias]
				if not f then
					error("reference to field "..alias.." which doesn't exist")
                end
				local mask = math.pow(2, f.size) - 1
				rulesfp:write(" | (($", wordindex,
                    " & ", string.format("0x%x", mask), ") << ", f.pos, ")")
			end
		end
	end
	rulesfp:write("); }\n")
end
rulesfp:close()

