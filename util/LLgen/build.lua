cprogram {
	name = "llgen",

	-- These use pre-LLgen'd versions of LLgen.c, Lpars.c and tokens.c. If
	-- LLgen.g gets updated, they need rebuilding. Use the bootstrap script to
	-- do this.

	srcs = { "./src/*.c" },
	cflags = { "-DLIBDIR=\\\""..posix.getcwd().."/"..cwd().."/lib\\\"", "-DNON_CORRECTING" },
}

definerule("llgen",
	{
		srcs = { type="targets" },
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		local fs = replace(basename(e.srcs), "%.g$", "")

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			outleaves = {
				"Lpars.c",
				"Lpars.h",
				unpack(replace(fs, "$", ".c"))
			},
			ins = {
				"util/LLgen+llgen",
				unpack(e.srcs),
			},
			commands = {
				"cd %{dir} && %{abspath(ins)}"
			}
		}
	end
)

--[[
D := util/LLgen

# Rule to build LLgen.

define build-llgen-impl
	$(call reset)
	$(eval cflags += -DNON_CORRECTING -DLIBDIR=\"$(abspath $D/lib)\")
	$(call cfile, $D/src/main.c)
	$(call cfile, $D/src/gencode.c)
	$(call cfile, $D/src/compute.c)
	$(call cfile, $D/src/check.c)
	$(call cfile, $D/src/reach.c)
	$(call cfile, $D/src/global.c)
	$(call cfile, $D/src/name.c)
	$(call cfile, $D/src/sets.c)
	$(call cfile, $D/src/alloc.c)
	$(call cfile, $D/src/machdep.c)
	$(call cfile, $D/src/cclass.c)
	$(call cfile, $D/src/savegram.c)

	# These use pre-LLgen'd version of the files. If LLgen.g gets updated,
	# they need rebuilding. Use the bootstrap script to do this.

	$(call cfile, $D/src/LLgen.c)
	$(call cfile, $D/src/Lpars.c)
	$(call cfile, $D/src/tokens.c)

	$(call cprogram, $(BINDIR)/LLgen)
	LLGEN := $o
endef

$(eval $(build-llgen-impl))

# Rule to invoke to *use* LLgen.
#
#  $1: directory to put output files
#  $2: input files
#
# Output files are compiled via cfile and queued.

define llgen-impl
$(eval o := $1/Lpars.c $(patsubst %.g, $(strip $1)/%.c, $(notdir $2)))
$(eval CLEANABLES += $o $1/Lpars.h)

$o: $1/Lpars.h
$1/Lpars.h: $2 $(LLGEN)
	@echo LLGEN $1/Lpars.c
	@mkdir -p $1
	$(hide) $(RM) $o $1/Lpars.h
	$(hide) cd $1 && $(LLGEN) $(abspath $2)

$(foreach f,$o,$(call cfile,$f))

endef

llgen = $(eval $(call llgen-impl,$1,$2))
--]]
