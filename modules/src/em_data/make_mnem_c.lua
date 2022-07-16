require "h.em_table_lib"

local specs, pseudos, mnems = load_table()

print("char em_mnem[][4] = {")
for _, mnem in ipairs(mnems) do
	print(string.format('\t"%s",', mnem.name))
end
print("};")
