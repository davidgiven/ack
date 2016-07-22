local function simpleprogram(name)
	cprogram {
		name = name,
		srcs = { "./"..name..".c" },
		deps = {
			"h+emheaders",
			"modules/src/object+lib",
		}
	}

	installable {
		name = name.."-pkg",
		map = {
			["$(INSDIR)/bin/"..name] = "+"..name,
			["$(PLATIND)/man/man1/"..name..".1"] = "./"..name..".1",
		}
	}
end

simpleprogram("aelflod")
simpleprogram("anm")
simpleprogram("ashow")
simpleprogram("asize")
simpleprogram("aslod")
simpleprogram("astrip")

installable {
	name = "pkg",
	map = {
		"+aelflod-pkg",
		"+anm-pkg",
		"+ashow-pkg",
		"+asize-pkg",
		"+aslod-pkg",
		"+astrip-pkg",
	}
}

