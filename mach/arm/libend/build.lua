for _, plat in ipairs(concat(vars.emplats, vars.pccplats)) do
	acklibrary {
		name = "lib_"..plat,
		srcs = { "./*.s" },
		vars = { plat = plat },
	}
end

