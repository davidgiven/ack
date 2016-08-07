for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = { "./*.s" },
		vars = { plat = plat },
	}
end

