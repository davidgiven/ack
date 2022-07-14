for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "headers_"..plat,
	}

	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./aar4.s",
			"./and.s",
			"./bls4.s",
			"./cfi8.s",
			"./cfu8.s",
			"./cif8.s",
			"./cms.s",
			"./com.s",
			"./csa.s",
			"./csb.s",
			"./cuf8.s",
			-- "./dus4.s",
			"./exg.s",
			"./fd_00000000.s",
			"./fef4.s",
			"./fef8.s",
			"./fif4.s",
			"./fif8.s",
			"./inn.s",
			"./ior.s",
			"./lar4.s",
			"./los4.s",
			"./rck.s",
			"./sar4.s",
			"./set.s",
			"./sts4.s",
			"./trp.s",
			"./xor.s",
			"./zer.s",
		},
		vars = { plat = plat },
		deps = {
			"h+emheaders",
			"+headers_"..plat,
		}
	}
end
