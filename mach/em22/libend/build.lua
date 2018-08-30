for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = { "./*.e" },
		vars = { plat = plat },
	}
end

