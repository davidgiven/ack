# (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
# See the copyright notice in the ACK home directory, in the file "Copyright".
#
# $Header$

EMHOME =	/usr/em
LINT =		/usr/dick/lint/lpass2/lint

CURSESDIR =	/usr/src/lib/libcurses#		# for a local -lcurses
MATHDIR =	/usr/src/lib/libm#		# for a local -lm
TERMLIBDIR =	/usr/src/lib/libtermlib#	# for a local -ltermlib

ALLDIR =	$(EMHOME)/lang/cem/libcc#	# to create c.llb
INCLUDE =	$(EMHOME)/include/_tail_cc
ALLFLAGS =	-D__BSD4_2 -I$(INCLUDE)#	# to compile $(ALLDIR)

c.llb:	unix7.c
	$(LINT) -Lc.llb unix7.c

curses.llb:
	$(LINT) -Lcurses.llb $(CURSESDIR)/*.c


m.llb:
	$(LINT) -Lm.llb $(MATHDIR)/*.c


termlib.llb:
	$(LINT) -Ltermlib.llb $(TERMLIBDIR)/*.c

cnew.llb:
	cp /dev/null cnew.llb
	$(LINT) -Lcnew.llb $(ALLFLAGS) $(ALLDIR)/*/*.c
	$(LINT) -Lcnew.llb unix7.c

