for _, plat in ipairs(vars.emplats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = { "./*.s" },
		vars = { plat = plat },
	}
end

