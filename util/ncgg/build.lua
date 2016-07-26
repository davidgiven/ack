include("first/yacc.lua")

local cggparser = yacc {
	name = "cggparser",
	srcs = { "./cgg.y" }
}

flex {
	name = "cgglexer",
	srcs = { "./scan.l" }
}

normalrule {
	name = "keywords",
	ins = {
		"./cvtkeywords",
		"./keywords",
		unpack(filenamesof({cggparser}, "%.h$"))
	},
	outleaves = { "enterkeyw.c" },
	commands = {
		"%{ins[1]} %{ins[2]} %{ins[3]} %{outs[1]}"
	}
}

cprogram {
	name = "ncgg",
	srcs = {
		"./*.c",
		"+cggparser", -- for .c file
		"+cgglexer", -- for .c file
		"+keywords",
	},
	deps = {
		"+cggparser", -- for .h file
		"+cgglexer", -- for .h file
		"h+emheaders",
		"util/data+em_data",
	}
}
	
definerule("ncgg",
	{
		srcs = { type="targets" }
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		if (#e.srcs ~= 1) then
			error("you must supply exactly one input file")
		end

		local cpptable = cppfile {
			name = e.name.."/cpptable",
			outleaf = "cpptable",
			srcs = e.srcs
		}

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			outleaves = {
				"tables.c",
				"tables.h",
			},
			ins = {
				"util/ncgg+ncgg",
				cpptable
			},
			commands = {
				"cd %{dir} && %{ins}",
				"mv %{dir}/tables.H %{dir}/tables.h"
			}
		}
	end
)

--[[
D := util/ncgg

define build-ncgg-impl

$(call reset)
$(eval cflags += -I$D)

$(call yacc, $(OBJDIR)/$D, $D/cgg.y)

$(call flex, $(OBJDIR)/$D, $D/scan.l)
$(call dependson, $(OBJDIR)/$D/y.tab.h)

$(call cfile, $D/subr.c)
$(call cfile, $D/main.c)
$(call cfile, $D/coerc.c)
$(call cfile, $D/error.c)
$(call cfile, $D/emlookup.c)
$(call cfile, $D/expr.c)
$(call cfile, $D/instruct.c)
$(call cfile, $D/iocc.c)
$(call cfile, $D/lookup.c)
$(call cfile, $D/output.c)
$(call cfile, $D/set.c)
$(call cfile, $D/strlookup.c)
$(call cfile, $D/var.c)
$(call cfile, $D/hall.c)

$(eval CLEANABLES += $(OBJDIR)/$D/enterkeyw.c)
$(OBJDIR)/$D/enterkeyw.c: $D/cvtkeywords $D/keywords $(OBJDIR)/$D/y.tab.h
	@echo KEYWORDS $$@
	@mkdir -p $$(dir $$@)
	$(hide) cd $$(dir $$@) && sh $(abspath $D/cvtkeywords) $(abspath $D/keywords)
$(call cfile, $(OBJDIR)/$D/enterkeyw.c)

$(eval $q: $(INCDIR)/em_spec.h)

$(call rawfile, $(LIBEM_DATA))
$(call cprogram, $(BINDIR)/ncgg)
$(eval NCGG := $o)

endef

$(eval $(build-ncgg-impl))
--]]
