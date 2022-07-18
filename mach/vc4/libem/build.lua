for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./csa.s",
			"./csb.s",
		},
		vars = { plat = plat },
	}
end

