require "h.em_table_lib"

local specs, pseudos, mnems = load_table()

print("char em_pseu[][4] = {")
for _, pseu in ipairs(pseudos) do
	print(string.format('\t"%s",', pseu.name))
end
print("};")
