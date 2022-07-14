include("plat/build.lua")

for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"./abi.c",
			"./abl.c",
			"./abr.c",
			"./arg.c",
			"./ass.c",
			"./asz.c",
			"./atn.c",
			"./bcp.c",
			"./buff.c",
			"./catch.c",
			"./cls.c",
			"./cvt.c",
			"./diag.c",
			"./efl.c",
			"./eln.c",
			"./exp.c",
			"./get.c",
			"./hlt.c",
			"./incpt.c",
			"./ini.c",
			"./log.c",
			"./mdi.c",
			"./mdl.c",
			"./new.c",
			"./nfa.c",
			"./nobuff.c",
			"./notext.c",
			"./opn.c",
			"./outcpt.c",
			"./pac.c",
			"./pclose.c",
			"./pcreat.c",
			"./pentry.c",
			"./perrno.c",
			"./pexit.c",
			"./popen.c",
			"./put.c",
			"./rcka.c",
			"./rdc.c",
			"./rdi.c",
			"./rdl.c",
			"./rdr.c",
			"./rf.c",
			"./rln.c",
			"./rnd.c",
			"./sin.c",
			"./sqt.c",
			"./string.c",
			"./unp.c",
			"./uread.c",
			"./uwrite.c",
			"./wdw.c",
			"./wf.c",
			"./wrc.c",
			"./wrf.c",
			"./wri.c",
			"./wrl.c",
			"./wrr.c",
			"./wrs.c",
			"./wrz.c",
			"./bts.e",
			"./encaps.e",
			"./fef.e",
			"./fif.e",
			"./gto.e",
			"./hol0.e",
			"./sig.e",
			"./trap.e",
			"./trp.e",
        },
		hdrs = {}, -- must be empty
		deps = {
			"h+emheaders",
			"lang/cem/libcc.ansi/headers+pkg",
			"lang/pc/include+headers",
			"plat/"..plat.."/include+pkg",
		},
        vars = { plat = plat }
    }

	ackfile {
		name = "prt_"..plat,
		srcs = { "./head_pc.e" },
		vars = { plat = plat },
		deps = {
			"h+emheaders",
			"lang/pc/include+headers",
		}
	}

	local suffix = plat:find("^em") and "m" or "o"
	installable {
		name = "pkg_"..plat,
		map = {
			["$(PLATIND)/"..plat.."/pascal."..suffix] = "+prt_"..plat,
			["$(PLATIND)/"..plat.."/libpascal.a"] = "+lib_"..plat,
		}
	}
end


