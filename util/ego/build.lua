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

cprogram {
	name = "bo",
	srcs = {
		"./bo/bo.c",
	},	
	deps = {
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "ca",
	srcs = {
		"./ca/ca.c",
		"./ca/ca_put.c",
	},	
	deps = {
		"./ca/ca.h",
		"./ca/ca_put.h",
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"modules/src/system+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "cf",
	srcs = {
		"./cf/cf.c",
		"./cf/cf_idom.c",
		"./cf/cf_loop.c",
		"./cf/cf_succ.c",

	},	
	deps = {
		"./cf/cf.h",
		"./cf/cf_idom.h",
		"./cf/cf_loop.h",
		"./cf/cf_succ.h",
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "cj",
	srcs = {
		"./cj/cj.c",
	},	
	deps = {
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "cs",
	srcs = {
		"./cs/cs_alloc.c",
		"./cs/cs_aux.c",
		"./cs/cs_avail.c",
		"./cs/cs.c",
		"./cs/cs_debug.c",
		"./cs/cs_elim.c",
		"./cs/cs_entity.c",
		"./cs/cs_getent.c",
		"./cs/cs_kill.c",
		"./cs/cs_partit.c",
		"./cs/cs_profit.c",
		"./cs/cs_stack.c",
		"./cs/cs_vnm.c",
	},	
	deps = {
		"./cs/cs_alloc.h",
		"./cs/cs_aux.h",
		"./cs/cs_avail.h",
		"./cs/cs_debug.h",
		"./cs/cs_elim.h",
		"./cs/cs_entity.h",
		"./cs/cs_getent.h",
		"./cs/cs.h",
		"./cs/cs_kill.h",
		"./cs/cs_partit.h",
		"./cs/cs_profit.h",
		"./cs/cs_stack.h",
		"./cs/cs_vnm.h",
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "ic",
	srcs = {
		"./ic/ic_aux.c",
		"./ic/ic.c",
		"./ic/ic_io.c",
		"./ic/ic_lib.c",
		"./ic/ic_lookup.c",
	},	
	deps = {
		"./ic/ic_aux.h",
		"./ic/ic.h",
		"./ic/ic_io.h",
		"./ic/ic_lib.h",
		"./ic/ic_lookup.h",
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "il",
	srcs = {
		"./il/il1_anal.c",
		"./il/il1_aux.c",
		"./il/il1_cal.c",
		"./il/il1_formal.c",
		"./il/il2_aux.c",
		"./il/il3_aux.c",
		"./il/il3_change.c",
		"./il/il3_subst.c",
		"./il/il_aux.c",
		"./il/il.c",
	},	
	deps = {
		"./il/il1_anal.h",
		"./il/il1_aux.h",
		"./il/il1_cal.h",
		"./il/il1_formal.h",
		"./il/il2_aux.h",
		"./il/il3_aux.h",
		"./il/il3_change.h",
		"./il/il3_subst.h",
		"./il/il_aux.h",
		"./il/il.h",
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "lv",
	srcs = {
		"./lv/lv.c",
	},	
	deps = {
		"./lv/lv.h",
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "sp",
	srcs = {
		"./sp/sp.c",
	},	
	deps = {
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "sr",
	srcs = {
		"./sr/sr_aux.c",
		"./sr/sr.c",
		"./sr/sr_cand.c",
		"./sr/sr_expr.c",
		"./sr/sr_iv.c",
		"./sr/sr_reduce.c",
		"./sr/sr_xform.c",
	},	
	deps = {
		"./sr/sr_aux.h",
		"./sr/sr_cand.h",
		"./sr/sr_expr.h",
		"./sr/sr.h",
		"./sr/sr_iv.h",
		"./sr/sr_reduce.h",
		"./sr/sr_xform.h",
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

cprogram {
	name = "ud",
	srcs = {
		"./ud/ud_aux.c",
		"./ud/ud.c",
		"./ud/ud_const.c",
		"./ud/ud_copy.c",
		"./ud/ud_defs.c",
	},
	deps = {
		"./ud/ud_aux.h",
		"./ud/ud_const.h",
		"./ud/ud_copy.h",
		"./ud/ud_defs.h",
		"./ud/ud.h",
		"util/ego/share+lib",
		"modules/src/em_data+lib",
		"h+emheaders",
	},
	vars = {
		["+cflags"] = {"-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"}
	}
}

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
