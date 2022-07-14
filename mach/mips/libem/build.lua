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
			"./cms.s",
			"./compareul.s",
			"./com.s",
			"./csa.s",
			"./csb.s",
			"./c_ud_i.s",
			"./c_uf_i.s",
			"./c_ui_d.s",
			"./c_ui_f.s",
			"./dus4.s",
			"./exg.s",
			"./fef8.s",
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
