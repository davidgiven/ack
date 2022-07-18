require "h.em_table_lib"

local specs, pseudos, mnems = load_table()

for _, mnem in ipairs(mnems) do
	if mnem.flags1:find("^[cslndfzor]") then
		print(string.format("#define C_%s(c) CC_opcst(op_%s, c)", mnem.name, mnem.name))
	elseif mnem.flags1:find("^w") then
		print(string.format("#define C_%s(w) CC_opcst(op_%s, w)", mnem.name, mnem.name))
		print(string.format("#define C_%s_narg() CC_opnarg(op_%s)", mnem.name, mnem.name))
	elseif mnem.flags1:find("^g") then
		print(string.format("#define C_%s(g) CC_opcst(op_%s, g)", mnem.name, mnem.name))
		print(string.format("#define C_%s_dnam(g, o) CC_opdnam(op_%s, g, o)", mnem.name, mnem.name))
		print(string.format("#define C_%s_dlb(g, o) CC_opdlb(op_%s, g, o)", mnem.name, mnem.name))
	elseif mnem.flags1:find("^p") then
		print(string.format("#define C_%s(p) CC_oppnam(op_%s, p)", mnem.name, mnem.name))
	elseif mnem.flags1:find("^b") then
		print(string.format("#define C_%s(b) CC_opilb(op_%s, b)", mnem.name, mnem.name))
	elseif mnem.flags1:find("^-") then
		print(string.format("#define C_%s() CC_op(op_%s)", mnem.name, mnem.name))
	else
		error("unsupport em_table flag")
	end
end

