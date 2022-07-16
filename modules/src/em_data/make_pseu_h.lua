require "h.em_table_lib"

local specs, pseudos, mnems = load_table()

for _, pseu in ipairs(pseudos) do
	print(string.format("#define ps_%s %d", pseu.name, pseu.num))
end
