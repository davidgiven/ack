require "modules.src.em_data.em_table_lib"

local specs, pseudos, mnems = load_table()

for _, spec in ipairs(specs) do
	print(string.format("#define sp_%s %d", spec.name, spec.num))
end
