require "h.em_table_lib"

local specs, pseudos, mnems = load_table()

local function makecase(flags, args)
	for _, mnem in ipairs(mnems) do
		if mnem.flags1:find(flags) then
			print(string.format('\tcase op_%s:\n\t\tC_%s%s;\n\t\tbreak;',
				mnem.name, mnem.name, args))
		end
	end
end

print("switch(p->em_opcode) {")

print("\t/* no arguments */")
makecase("^-", "()")

print("\t/* one integer constant argument */")
makecase("^[cdflnorswz]", "(p->em_cst)")

print("\t/* a procedure name argument */")
makecase("^p", "(p->em_pnam)")

print("\t/* An instruction label argument */")
makecase("^b", "((label) (p->em_cst))")

print([[
	default:
	/* a "g" argument */
		if (p->em_argtype == nof_ptyp) {
			switch(p->em_opcode) {
				default:
					EM_error = "Illegal mnemonic";
					break;
]])
for _, mnem in ipairs(mnems) do
	if mnem.flags1:find("^g") then
		print(string.format('\t\t\t\tcase op_%s:', mnem.name))
		print(string.format('\t\t\t\t\tC_%s_dlb(p->em_dlb, p->em_off);\n\t\t\t\t\tbreak;',
			mnem.name))
	end
end
print([[
			}
		}
		else if (p->em_argtype == sof_ptyp) {
			switch(p->em_opcode) {
				default:
					EM_error = "Illegal mnemonic";
					break;
]])
for _, mnem in ipairs(mnems) do
	if mnem.flags1:find("^g") then
		print(string.format('\t\t\t\tcase op_%s:', mnem.name))
		print(string.format('\t\t\t\t\tC_%s_dnam(p->em_dnam, p->em_off);\n\t\t\t\t\tbreak;',
			mnem.name))
	end
end
print([[
			}
		}
		else /*argtype == cst_ptyp */ {
			switch(p->em_opcode) {
				default:
					EM_error = "Illegal mnemonic";
					break;
]])
for _, mnem in ipairs(mnems) do
	if mnem.flags1:find("^g") then
		print(string.format('\t\t\t\tcase op_%s:', mnem.name))
		print(string.format('\t\t\t\t\tC_%s(p->em_cst);\n\t\t\t\t\tbreak;',
			mnem.name))
	end
end
print([[
			}
		}
}
]])
