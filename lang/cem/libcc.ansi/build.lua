for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
            "./assert/*.c",
        },
        hdrs = {
        },
        vars = { plat = plat }
    }

	ackfile {
		name = "crt_"..plat,
		srcs = { "./head_ac.e" },
		vars = { plat = plat },
		deps = {
			"h+emheaders"
		}
	}

	installable {
		name = "pkg_"..plat,
		map = {
			["$(PLATIND)/"..plat.."/c-ansi.o"] = "+crt_"..plat,
			--["$(PLATIND)/"..plat.."/libc.a"] = "+lib_"..plat,
		}
	}
end

