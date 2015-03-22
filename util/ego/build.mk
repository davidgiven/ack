D := util/ego

define build-ego-backend-impl

$(call reset)
$(eval cflags += -DVERBOSE -DNOTCOMPACT)
$(eval cflags += -I$D/share -I$(OBJDIR)/$D)
$(foreach f, $2, $(call cfile, $f))
$(call rawfile, $(LIBDIR)/libegocore.a)
$(call rawfile, $(LIBEM_DATA))
$(call cprogram, $(BINDIR)/ego/$(strip $1))
$(call installto, $(PLATDEP)/ego/$(strip $1))
$(eval EGO_MODULES += $q)

endef

define build-ego-impl

# Generated files that egocore needs.

$(call reset)
$(call cfile, $D/share/makecldef.c)
$(call cprogram, $(OBJDIR)/$D/makecldef)

$(eval g := $(OBJDIR)/$D/classdefs.h)
$(eval CLEANABLES += $g)
$g: $(OBJDIR)/$D/makecldef $(INCDIR)/em_mnem.h $D/share/cldefs.src
	@echo MAKECLDEF $g
	@mkdir -p $(dir $g)
	$(hide) $$^ > $g

$(eval g := $(OBJDIR)/$D/pop_push.h)
$(eval CLEANABLES += $g)
$g: $D/share/pop_push.awk h/em_table
	@echo POP_PUSH $g
	@mkdir -p $(dir $g)
	$(hide) awk -f $D/share/pop_push.awk < h/em_table > $g

# Build the egocore library.

$(call reset)
$(eval cflags += -I$D/share -I$(OBJDIR)/$D)
$(eval cflags += -DVERBOSE -DNOTCOMPACT)

$(call cfile, $D/share/debug.c)
$(call cfile, $D/share/global.c)
$(call cfile, $D/share/files.c)
$(call cfile, $D/share/go.c)
$(call cfile, $D/share/map.c)
$(call cfile, $D/share/aux.c)
$(call cfile, $D/share/get.c)
$(call cfile, $D/share/put.c)
$(call cfile, $D/share/alloc.c)
$(call cfile, $D/share/lset.c)
$(call cfile, $D/share/cset.c)
$(call cfile, $D/share/parser.c)
$(call cfile, $D/share/stack_chg.c)
$(call cfile, $D/share/locals.c)
$(call cfile, $D/share/init_glob.c)
$(eval $q: $(OBJDIR)/$D/classdefs.h $(OBJDIR)/$D/pop_push.h)

$(call clibrary, $(LIBDIR)/libegocore.a)
$(eval CLEANABLES += $q)

# Now build each of the back ends.

$(call build-ego-backend-impl, bo, \
	$D/bo/bo.c)

$(call build-ego-backend-impl, ca, \
	$D/ca/ca.c \
	$D/ca/ca_put.c)

$(call build-ego-backend-impl, cf, \
	$D/cf/cf.c \
	$D/cf/cf_idom.c \
	$D/cf/cf_loop.c \
	$D/cf/cf_succ.c)

$(call build-ego-backend-impl, cj, \
	$D/cj/cj.c)

$(call build-ego-backend-impl, cs, \
	$D/cs/cs.c \
	$D/cs/cs_alloc.c \
	$D/cs/cs_aux.c \
	$D/cs/cs_avail.c \
	$D/cs/cs_debug.c \
	$D/cs/cs_elim.c \
	$D/cs/cs_entity.c \
	$D/cs/cs_getent.c \
	$D/cs/cs_kill.c \
	$D/cs/cs_partit.c \
	$D/cs/cs_profit.c \
	$D/cs/cs_stack.c \
	$D/cs/cs_vnm.c)

$(call build-ego-backend-impl, ic, \
	$D/ic/ic.c \
	$D/ic/ic_aux.c \
	$D/ic/ic_io.c \
	$D/ic/ic_lib.c \
	$D/ic/ic_lookup.c)

$(call build-ego-backend-impl, il, \
	$D/il/il.c \
	$D/il/il1_anal.c \
	$D/il/il1_aux.c \
	$D/il/il1_cal.c \
	$D/il/il1_formal.c \
	$D/il/il2_aux.c \
	$D/il/il3_aux.c \
	$D/il/il3_change.c \
	$D/il/il3_subst.c \
	$D/il/il_aux.c)

$(call build-ego-backend-impl, lv, \
	$D/lv/lv.c)

$(call build-ego-backend-impl, ra, \
	$D/ra/ra.c \
	$D/ra/ra_allocl.c \
	$D/ra/ra_aux.c \
	$D/ra/ra_interv.c \
	$D/ra/ra_lifet.c \
	$D/ra/ra_pack.c \
	$D/ra/ra_profits.c \
	$D/ra/ra_xform.c \
	$D/ra/ra_items.c)

$(eval g := $(OBJDIR)/$D/itemtab.h)
$(eval CLEANABLES += $g)
$D/ra/ra_items.c: $g
$g: $(OBJDIR)/$D/makeitems $(INCDIR)/em_mnem.h $D/ra/itemtab.src
	@echo MAKEITEMS $g
	@mkdir -p $(dir $g)
	$(hide) $$^ > $g

$(call reset)
$(call cfile, $D/ra/makeitems.c)
$(call cprogram, $(OBJDIR)/$D/makeitems)

$(call build-ego-backend-impl, sp, \
	$D/sp/sp.c)

$(call build-ego-backend-impl, sr, \
	$D/sr/sr.c \
	$D/sr/sr_aux.c \
	$D/sr/sr_cand.c \
	$D/sr/sr_expr.c \
	$D/sr/sr_iv.c \
	$D/sr/sr_reduce.c \
	$D/sr/sr_xform.c)

$(call build-ego-backend-impl, ud, \
	$D/ud/ud.c \
	$D/ud/ud_aux.c \
	$D/ud/ud_const.c \
	$D/ud/ud_copy.c \
	$D/ud/ud_defs.c)

# ...and now the front end, which depends on all the backends.

$(call reset)
$(call cfile, $D/em_ego/em_ego.c)
$(eval $q: $(INCDIR)/print.h $(INCDIR)/system.h)
$(eval $q: $(INCDIR)/em_path.h)

$(call rawfile, $(LIBPRINT))
$(call rawfile, $(LIBSTRING))
$(call rawfile, $(LIBSYSTEM))

$(call cprogram, $(BINDIR)/em_ego)
$(call installto, $(PLATDEP)/em_ego)
$(eval EM_EGO := $q)
$(eval $q: $(EGO_MODULES))
$(eval ACK_CORE_TOOLS += $q)

endef

$(eval $(build-ego-impl))

# Install the ego descr file for the current architecture, if one exists.
# This is a bit annoying because it's called by each platform, so we need
# to protect against the architecture being seen more than once. And some
# architectures don't get descr files.

define build-ego-descr-impl

$(eval EGO_DESCR_$(ARCH) := 1)

$(eval g := $(PLATIND)/ego/$(ARCH).descr)
$(eval CLEANABLES += $g)
$(eval $(EM_EGO): $g)
$g: util/ego/descr/descr.sed util/ego/descr/$(ARCH).descr $(CPPANSI) \
		$(INCDIR)/em_mnem.h
	@echo EGODESCR $g
	@mkdir -p $(dir $g)
	$(hide) $(CPPANSI) -P -I$(INCDIR) util/ego/descr/$(ARCH).descr | sed -f util/ego/descr/descr.sed > $g

endef

build-ego-descr = $(if $(EGO_DESCR_$(ARCH)),, \
	$(if $(wildcard util/ego/descr/$(ARCH).descr), \
		$(eval $(call build-ego-descr-impl, $1))))