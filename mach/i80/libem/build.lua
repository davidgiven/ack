local generated = {}

definerule("generate",
	{
		body = { type="string" },
		offset = { type="object" },
	},
	function(e)
		return normalrule {
			name = e.name,
			ins = { "./generate.sh", "./"..e.body },
			outleaves = { e.name..".s" },
			commands = {
				"%{ins[1]} "..e.body.." "..e.offset.." > %{outs}"
			}
		}
	end
)
		
for i = 1, 128 do
	generated[#generated+1] = generate {
		name = "fload"..i,
		body = "fload.h",
		offset = i
	}

	generated[#generated+1] = generate {
		name = "floadn"..i,
		body = "floadn.h",
		offset = i
	}

	generated[#generated+1] = generate {
		name = "fstore"..i,
		body = "fstore.h",
		offset = i
	}

	generated[#generated+1] = generate {
		name = "fstoren"..i,
		body = "fstoren.h",
		offset = i
	}

	generated[#generated+1] = generate {
		name = "faddr"..i,
		body = "faddr.h",
		offset = i
	}

	generated[#generated+1] = generate {
		name = "faddrn"..i,
		body = "faddrn.h",
		offset = i
	}
end

for _, plat in ipairs(vars.plats) do
	acklibrary {
		name = "lib_"..plat,
		srcs = {
			"./aar2.s",
			"./adi4.s",
			"./and.s",
			"./blm.s",
			"./cii.s",
			"./cmi4.s",
			"./cmps_mag.s",
			"./cmpu_mag.s",
			"./cms.s",
			"./com.s",
			"./csa.s",
			"./csb.s",
			"./dup.s",
			"./dvi2.s",
			"./dvi4.s",
			"./exg.s",
			"./flp.s",
			"./inn.s",
			"./ior.s",
			"./lar2.s",
			"./loi.s",
			"./mli2.s",
			"./mli4.s",
			"./mlu2.s",
			"./ngi4.s",
			"./nop.s",
			"./pchl.s",
			"./pro.s",
			"./rck.s",
			"./ret.s",
			"./rol4.s",
			"./ror4.s",
			"./rst.s",
			"./sar2.s",
			"./sbi4.s",
			"./set2.s",
			"./set.s",
			"./sli2.s",
			"./sli4.s",
			"./sri2.s",
			"./sri4.s",
			"./sti.s",
			"./xor.s",
			generated
		},
		vars = {
			plat = plat,
			["+ackcflags"] = {"-Imach/i80/libem"}
		},
	}
end

