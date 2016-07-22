local posix = require("posix")
include("util/LLgen/build.lua")

normalrule {
	name = "parameters",
	ins = { "./BigPars" },
	outleaves = { "parameters.h" },
	commands = {
		"echo '#ifndef PARAMETERS_H' > %{outs}",
		"echo '#define PARAMETERS_H' >> %{outs}",
		"grep -v '^!' < %{ins} >> %{outs}",
		"echo '#endif' >> %{outs}"
	}
}

local str_files = basename(posix.glob(cwd().."/*.str"))
local str_targets = {}

for _, f in ipairs(str_files) do
	local bf = f:gsub("%.str$", "")
	str_targets[#str_targets+1] = normalrule {
		name = "allocd_header/"..bf,
		ins = { "./make.allocd", "./"..f },
		outleaves = { bf..".h" },
		commands = {
			"%{ins[1]} < %{ins[2]} > %{outs}"
		}
	}
end

normalrule {
	name = "next-c",
	ins = {
		"./make.next",
		"./*.str",
	},
	outleaves = { "next.c" },
	commands = {
		"%{ins} > %{outs[1]}"
	}
}

clibrary {
	name = "nextlib",
	srcs = { "+next-c" },
	hdrs = str_targets,
	deps = {
		"+parameters",
		unpack(str_targets)
	}
}

normalrule {
	name = "tokenname-g",
	ins = {
		"./make.tokfile",
		"./tokenname.c",
	},
	outleaves = { "tokenname.g" },
	commands = {
		"sh %{ins[1]} < %{ins[2]} > %{outs}",
	}
}

llgen {
	name = "llgen",
	srcs = {
		"+tokenname-g", -- must be first
		"./*.g",
	},
}

cprogram {
	name = "cemcom",
	srcs = {
		"./ch3.c",
		"./ch3bin.c",
		"./ch3mon.c",
		"./code.c",
		"./conversion.c",
		"./cstoper.c",
		"./dataflow.c",
		"./declarator.c",
		"./decspecs.c",
		"./domacro.c",
		"./dumpidf.c",
		"./error.c",
		"./eval.c",
		"./expr.c",
		"./field.c",
		"./fltcstoper.c",
		"./idf.c",
		"./init.c",
		"./input.c",
		"./l_comment.c",
		"./l_ev_ord.c",
		"./l_lint.c",
		"./l_misc.c",
		"./l_outdef.c",
		"./l_states.c",
		"./label.c",
		"./main.c",
		"./options.c",
		"./pragma.c",
		"./proto.c",
		"./replace.c",
		"./skip.c",
		"./stab.c",
		"./stack.c",
		"./struct.c",
		"./switch.c",
		"./tokenname.c",
		"./type.c",
		"./util.c",
	},
	deps = {
		"+parameters",
		"+nextlib",
		"+llgen",
		"h+emheaders",
		"modules/src/alloc+lib",
		"modules/src/em_code+lib_k",
		"modules/src/flt_arith+lib",
		"modules/src/idf+lib",
		"modules/src/input+lib",
		"modules/src/system+lib",
		"modules+headers",
		"util/data+em_data",
	},
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_cemcom.ansi"] = "+cemcom"
	}
}
--[[

D := lang/cem/cemcom.ansi

define build-cemcom-ansi-allocd-header
$(eval g := $(OBJDIR)/$D/$(strip $1).h)
$g: $D/$(strip $1).str $D/make.allocd
	@echo ALLOCD $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/make.allocd < $$^ > $$@

$(eval CLEANABLES += $g)
$(eval $q: $g)
endef

define build-cemcom-ansi-next
$(eval CLEANABLES += $(OBJDIR)/$D/next.c)
$(OBJDIR)/$D/next.c: $D/make.next $1
	@echo NEXT $$@
	@mkdir -p $$(dir $$@)
	$(hide) $$^ > $$@
$(call cfile, $(OBJDIR)/$D/next.c)

$(foreach f, $1, $(call build-cemcom-ansi-allocd-header, \
	$(basename $(notdir $f))))
endef

define build-cemcom-ansi-impl

$(call reset)
$(eval cflags += -I$(OBJDIR)/$D -I$D)

$(call cfile, $D/arith.c)
$(call dependson, $(INCDIR)/flt_arith.h)

$(call cfile, $D/blocks.c)
$(call dependson, $(INCDIR)/em_codeEK.h)

$(call cfile, $D/LLlex.c)
$(call cfile, $D/LLmessage.c)


$(call llgen, $(OBJDIR)/$D, \
	$(OBJDIR)/$D/tokenfile.g \
	$D/program.g \
	$D/declar.g \
	$D/expression.g \
	$D/statement.g \
	$D/ival.g)

$(eval CLEANABLES += $(OBJDIR)/$D/tokenfile.g)
$(OBJDIR)/$D/tokenfile.g: $D/make.tokfile $D/tokenname.c
	@echo TOKENFILE $$@
	@mkdir -p $$(dir $$@)
	$(hide) sh $D/make.tokfile < $D/tokenname.c > $$@

$(call tabgen, $D/char.tab)

$(eval $q: $(OBJDIR)/$D/parameters.h)

$(eval CLEANABLES += $(OBJDIR)/$D/parameters.h)
$(OBJDIR)/$D/parameters.h: $D/BigPars
	@echo PARAMETERS $$@
	@mkdir -p $$(dir $$@)
	$(hide) echo '#ifndef PARAMETERS_H' > $$@
	$(hide) echo '#define PARAMETERS_H' >> $$@
	$(hide) grep -v '^!' < $D/BigPars >> $$@
	$(hide) echo '#endif' >> $$@

$(eval CLEANABLES += $(OBJDIR)/$D/symbol2str.c)
$(OBJDIR)/$D/symbol2str.c: $D/make.tokcase $D/tokenname.c
	@echo TOKCASE $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/make.tokcase < $D/tokenname.c > $$@
$(call cfile, $(OBJDIR)/$D/symbol2str.c)

$(call build-cemcom-ansi-next, \
		$D/code.str \
		$D/declar.str \
		$D/def.str \
		$D/expr.str \
		$D/field.str \
		$D/estack.str \
		$D/util.str \
		$D/proto.str \
		$D/replace.str \
		$D/idf.str \
		$D/macro.str \
		$D/stack.str \
		$D/stmt.str \
		$D/struct.str \
		$D/switch.str \
		$D/type.str \
		$D/l_brace.str \
		$D/l_state.str \
		$D/l_outdef.str)

$(eval $q: $(OBJDIR)/$D/Lpars.h)

$(call rawfile, $(LIBEM_MES))
$(call rawfile, $(LIBEMK))
$(call rawfile, $(LIBEM_DATA))
$(call rawfile, $(LIBINPUT))
$(call rawfile, $(LIBASSERT))
$(call rawfile, $(LIBALLOC))
$(call rawfile, $(LIBFLT_ARITH))
$(call rawfile, $(LIBPRINT))
$(call rawfile, $(LIBSYSTEM))
$(call rawfile, $(LIBSTRING))
$(call cprogram, $(BINDIR)/cemcom.ansi)
$(call installto, $(PLATDEP)/em_cemcom.ansi)
$(eval CEMCOMANSI := $o)

$(call reset)
$(eval q := $D/cemcom.ansi.1)
$(call installto, $(INSDIR)/share/man/man1/cemcom.6)
endef

$(eval $(build-cemcom-ansi-impl))
--]]
