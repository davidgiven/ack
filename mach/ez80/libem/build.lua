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
			"./cii36.s",
			"./extendb.s",
			"./float.s",
			"./gto.s",
			"./ignmask.s",
			"./ior3.s",
			"./ior6.s",
			"./mli3.s",
			"./mli6.s",
			"./not3.s",
			"./pchl.s",
			"./rmsdvs3.s",
			"./rmsdvs6.s",
			"./rmudvu3.s",
			"./rmudvu6.s",
			"./sli3.s",
			"./sru3.s",
			"./testhl3.s",
			"./testhlhl6.s",
			"./trppc.s",
		},
		vars = { plat = plat },
		deps = {
			"h+emheaders",
			"+internal",
		}
	}
end
