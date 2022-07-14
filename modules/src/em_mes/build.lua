clibrary {
	name = "lib",
	srcs = {
		"./C_ms_com.c",
		"./C_ms_ego.c",
		"./C_ms_emx.c",
		"./C_ms_err.c",
		"./C_ms_flt.c",
		"./C_ms_gto.c",
		"./C_ms_opt.c",
		"./C_ms_par.c",
		"./C_ms_reg.c",
		"./C_ms_src.c",
		"./C_ms_stb.c",
		"./C_ms_std.c",
	},
	deps = {
		"h+emheaders",
		"modules+headers",
		"modules/src/em_code+headers",
		"modules/src/em_data+lib",
	}
}

