# (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
# See the copyright notice in the ACK home directory, in the file "Copyright".
#
# $Header$

EM =		/usr/em
LINT =		/usr/dick/lint/lint

ALLDIR =	$(EM)/lang/cem/libcc#		# to create llib-lc
CURSESDIR =	/usr/src/lib/libcurses#		# for a local -lcurses
MATHDIR =	/usr/src/lib/libm#		# for a local -lm
TERMLIBDIR =	/usr/src/lib/libtermlib#	# for a local -ltermlib

llib-lc:	unix7.c
	$(LINT) -Lllib-lc unix7.c

llib-lcurses:
	$(LINT) -Lllib-lcurses $(CURSESDIR)/*.c


llib-lm:
	$(LINT) -Lllib-lm $(MATHDIR)/*.c


llibl-termlib:
	$(LINT) -Lllibl-termlib $(TERMLIBDIR)/*.c

llib-lcnew:
	cp /dev/null llib-lc
	$(LINT) -Lllibl-lc $(ALLDIR)/*/*.c

