require "h.em_table_lib"

local specs, pseudos, mnems = load_table()

print("switch(p->em_opcode) {")
for _, mnem in ipairs(mnems) do
	if mnem.flags1:find("^w") then
		print(string.format('\tcase op_%s:', mnem.name))
		print(string.format('\t\tC_%s_narg();\n\t\tbreak;',
			mnem.name))
	end
end
print([[
	default: EM_error = "Illegal mnemonic";
}
]])
