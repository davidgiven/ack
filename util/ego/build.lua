local function build_ego(name)
	cprogram {
		name = name,
		srcs = { "./"..name.."/*.c" },
		deps = {
			"./"..name.."/*.h",
			"util/ego/share+lib",
			"modules/src/em_data+lib",
			"h+emheaders",
		},
		vars = {
			["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
		}
	}
end

build_ego("bo")
build_ego("ca")
build_ego("cf")
build_ego("cj")
build_ego("cs")
build_ego("ic")
build_ego("il")
build_ego("lv")
build_ego("sp")
build_ego("sr")
build_ego("ud")

cprogram {
	name = "em_ego",
	srcs = { "./em_ego/em_ego.c" },
	deps = {
		"modules/src/print+lib",
		"modules/src/string+lib",
		"modules/src/system+lib",
		"modules+headers",
		"h+emheaders",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_ego"] = "+em_ego",
		["$(PLATDEP)/ego/bo"] = "+bo",
		["$(PLATDEP)/ego/ca"] = "+ca",
		["$(PLATDEP)/ego/cf"] = "+cf",
		["$(PLATDEP)/ego/cj"] = "+cj",
		["$(PLATDEP)/ego/cs"] = "+cs",
		["$(PLATDEP)/ego/ic"] = "+ic",
		["$(PLATDEP)/ego/il"] = "+il",
		["$(PLATDEP)/ego/lv"] = "+lv",
		["$(PLATDEP)/ego/ra"] = "./ra+ra",
		["$(PLATDEP)/ego/sp"] = "+sp",
		["$(PLATDEP)/ego/sr"] = "+sr",
		["$(PLATDEP)/ego/ud"] = "+ud",
		"./descr+pkg",
	}
}
