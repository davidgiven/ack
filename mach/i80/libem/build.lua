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
		srcs = concat("./*.s", generated),
		vars = {
			plat = plat,
			["+ackcflags"] = {"-Imach/i80/libem"}
		},
	}
end

