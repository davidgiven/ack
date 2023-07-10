acklibrary {
    name = "internal",
    hdrs = {
		"./asm.h"
	}
}

local trap_calls = {
   "EARRAY",
    "EBADGTO",
    "EBADLAE",
    "EBADLIN",
    "EBADMON",
    "EBADPC",
    "EBADPTR",
    "ECASE",
    "ECONV",
    "EFDIVZ",
    "EFOVFL",
    "EFUND",
    "EFUNFL",
    "EHEAP",
    "EIDIVZ",
    "EILLINS",
    "EIOVFL",
    "EIUND",
    "EMEMFLT",
    "EODDZ",
    "ERANGE",
    "ESET",
    "ESTACK",
    "EUNIMPL",
}

local generated = {}
for _, name in pairs(trap_calls) do
    generated[#generated+1] = normalrule {
        name = name,
        ins = { "./make_trap.sh" },
        outleaves = { name..".s" },
        commands = {
            "%{ins[1]} "..name:lower().." "..name.." > %{outs}"
        }
    }
end

for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = {
			generated,
			"./and.s",
			"./and3.s",
			"./and6.s",
			"./bls.s",
			"./cms.s",
			"./dus.s",
			"./exg.s",
			"./extendb.s",
			"./extendw.s",
			"./float.s",
			"./gto.s",
			"./hol0.s",
			"./ignmask.s",
			"./inn.s",
			"./loi.s",
			"./sti.s",
			"./ior.s",
			"./ior3.s",
			"./ior6.s",
			"./mli3.s",
			"./mli6.s",
			"./not3.s",
			"./not6.s",
			"./pchl.s",
			"./rmsdvs3.s",
			"./rmsdvs6.s",
			"./rmudvu3.s",
			"./rmudvu6.s",
			"./set.s",
			"./sli3.s",
			"./sli6.s",
			"./sri3.s",
			"./sri6.s",
			"./sru3.s",
			"./sru6.s",
			"./signedcmp.s",
			"./trppc.s",
			"./xor.s",
			"./xor3.s",
			"./xor6.s",
		},
		vars = { plat = plat },
		deps = {
			"h+emheaders",
			"+internal",
		}
	}
end
