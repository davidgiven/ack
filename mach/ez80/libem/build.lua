acklibrary {
    name = "internal",
    hdrs = {
		"./asm.h"
	}
}

for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./and3.s",
			"./and6.s",
			"./bls.s",
			"./cii36.s",
			"./extendb.s",
			"./float.s",
			"./gto.s",
			"./hol0.s",
			"./ignmask.s",
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
			"./sli3.s",
			"./sli6.s",
			"./sru3.s",
			"./sru6.s",
			"./testhl3.s",
			"./testhlhl6.s",
			"./trppc.s",
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
