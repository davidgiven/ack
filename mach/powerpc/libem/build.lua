for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "headers_"..plat,
		hdrs = { "./*.h" }
	}

	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./*.s",
		},
		vars = { plat = plat },
		deps = {
			"h+emheaders",
			"+headers_"..plat,
		}
	}
end

