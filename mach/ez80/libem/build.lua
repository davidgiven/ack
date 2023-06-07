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
			"./float.s",
			"./gto.s",
			"./ignmask.s",
			"./ior3.s",
			"./ior6.s",
			"./pchl.s",
			"./rmudvu3.s",
			"./sli3.s",
			"./testbc3.s",
			"./testbcbc6.s",
			"./testde3.s",
			"./testdede6.s",
			"./testhl3.s",
			"./testhlhl6.s",
		},
		vars = { plat = plat },
		deps = {
			"h+emheaders",
			"+internal",
		}
	}
end
