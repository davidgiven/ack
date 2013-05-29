D := util/ack

define util-ack-makeheaders-rule
$(eval g := $(OBJDIR)/$D/dmach.c $(OBJDIR)/$D/intable.c)
$(wordlist 2, $(words $g), $g): $(firstword $g)
$(firstword $g): $(util-ack-mktables)
	@echo MKTABLES
	@mkdir -p $(OBJDIR)/$D
	$(hide) cd $(OBJDIR)/$D && $(util-ack-mktables) $(INSDIR)/share

$(eval CLEANABLES += $g)
endef

define build-ack-impl
	$(call reset)
	$(call cfile, $D/mktables.c)
	$(call cprogram, $(OBJDIR)/$D/mktables)
	$(eval util-ack-mktables := $o)

	$(call reset)
	$(eval cflags += -I$D)
	$(call cfile, $D/list.c)
	$(call cfile, $D/data.c)
	$(call cfile, $D/main.c)
	$(call cfile, $D/scan.c)
	$(call cfile, $D/svars.c)
	$(call cfile, $D/trans.c)
	$(call cfile, $D/util.c)

	$(call cfile, $D/rmach.c)
	$(call dependson, $(INCDIR)/em_path.h)

	$(call cfile, $D/run.c)
	$(call cfile, $D/grows.c)

	$(call cfile, $D/files.c)
	$(call dependson, $(INCDIR)/em_path.h)

	$(eval $(util-ack-makeheaders-rule))
	$(call cfile, $(OBJDIR)/$D/dmach.c)
	$(call cfile, $(OBJDIR)/$D/intable.c)

	$(call cprogram, $(BINDIR)/ack)
	$(call installto, $(INSDIR)/bin/ack)
	$(eval ACK := $o)

	$(call reset)
	$(eval q := lib/descr/fe)
	$(call installto, $(PLATIND)/descr/fe)
	$(eval $(ACK): $o)

	$(call reset)
	$(eval q := $D/ack.1.X)
	$(call installto, $(INSDIR)/share/man/man1/ack.1)
endef

$(eval $(build-ack-impl))
