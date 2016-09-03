for _, plat in ipairs(vars.emplats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./*.s",
			"./*.c"
		},
		vars = { plat = plat },
		deps = {
			"h+emheaders"
		}
	}
end

