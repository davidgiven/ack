for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./edata.s",
			"./em_end.s",
			"./end.s",
			"./etext.s",
		},
		vars = { plat = plat },
	}
end

