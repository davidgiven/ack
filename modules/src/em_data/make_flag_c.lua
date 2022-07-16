require "modules.src.em_data.em_table_lib"

local specs, pseudos, mnems = load_table()

local function flag(c)
	if c == "-" then
		return "NO"
	else
		return c:upper()
	end
end

print("#include <em_flag.h>")
print("char em_flag[] = {")
for _, mnem in ipairs(mnems) do
	print(string.format("\tPAR_%s | FLO_%s,",
		flag(mnem.flags1:sub(1, 1)),
		flag(mnem.flags1:sub(2, 2))))
end
print("};")

