for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./aar.s",
			"./cii.s",
			"./cmi.s",
			"./cms.s",
			"./cmu.s",
			"./csa8.s",
			"./csa.s",
			"./csb8.s",
			"./csb.s",
			"./cuu.s",
			"./cvf.s",
			"./dia.s",
			"./divrem8.s",
			"./dvi8.s",
			"./dvu8.s",
			"./exg.s",
			"./fat.s",
			"./fp68881.s",
			"./inn.s",
			"./lar.s",
			"./los.s",
			"./mon.s",
			"./nop.s",
			"./rmi8.s",
			"./rmu8.s",
			"./sar.s",
			"./set.s",
			"./shp.s",
			"./sts.s",
			"./trp.s",
			"./trpstr.c",
		},
		vars = { plat = plat },
		deps = {
			"h+emheaders"
		}
	}
end

