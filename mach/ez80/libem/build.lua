for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "headers_"..plat,
	}

	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./gto.s",
		},
		vars = { plat = plat },
		deps = {
			"h+emheaders",
			"+headers_"..plat,
		}
	}
end
