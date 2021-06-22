local function simpleprogram(name)
	cprogram {
		name = name,
		srcs = { "./"..name..".c" },
		deps = {
			"h+emheaders",
			"modules/src/alloc+lib",
			"modules/src/data+lib",
			"modules/src/object+lib",
			"modules/src/system+lib",
		}
	}

	installable {
		name = name.."-pkg",
		map = {
			["$(INSDIR)/bin/"..name] = "+"..name,
			["$(INSDIR)/share/man/man1/"..name..".1"] = "./"..name..".1",
		}
	}
end

simpleprogram("abmodules")
simpleprogram("aelflod")
simpleprogram("amzlod")
simpleprogram("anm")
simpleprogram("ashow")
simpleprogram("asize")
simpleprogram("aslod")
simpleprogram("astrip")

installable {
	name = "pkg",
	map = {
		"+abmodules-pkg",
		"+aelflod-pkg",
		"+amzlod-pkg",
		"+anm-pkg",
		"+ashow-pkg",
		"+asize-pkg",
		"+aslod-pkg",
		"+astrip-pkg",
	}
}

