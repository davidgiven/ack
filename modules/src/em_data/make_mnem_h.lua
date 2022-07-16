require "modules.src.em_data.em_table_lib"

local specs, pseudos, mnems = load_table()

for _, mnem in ipairs(mnems) do
	print(string.format("#define op_%s %d", mnem.name, mnem.num))
end
