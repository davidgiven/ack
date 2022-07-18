for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./edata.e",
			"./em_end.e",
			"./end.e",
			"./etext.e",
		},
		vars = { plat = plat },
	}
end

