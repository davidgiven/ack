# $Header$

#PARAMS		do not remove this line!

SRC_DIR		= $(SRC_HOME)/util/byacc
DEST		= $(TARGET_HOME)/bin
MAN		= $(TARGET_HOME)/man

HDRS		= $(SRC_DIR)/defs.h

INCLUDES	= -I$(SRC_DIR)
CFLAGS		= $(COPTIONS) $(INCLUDES)
LDFLAGS		= $(LDOPTIONS)
LINTFLAGS	= $(LINTOPTIONS) $(INCLUDES)

OBJS	      = closure.$(SUF) \
		error.$(SUF) \
		lalr.$(SUF) \
		lr0.$(SUF) \
		main.$(SUF) \
		mkpar.$(SUF) \
		output.$(SUF) \
		reader.$(SUF) \
		skeleton.$(SUF) \
		symtab.$(SUF) \
		verbose.$(SUF) \
		warshall.$(SUF)

PROGRAM	      = yacc

SRCS	      = $(SRC_DIR)/closure.c \
		$(SRC_DIR)/error.c \
		$(SRC_DIR)/lalr.c \
		$(SRC_DIR)/lr0.c \
		$(SRC_DIR)/main.c \
		$(SRC_DIR)/mkpar.c \
		$(SRC_DIR)/output.c \
		$(SRC_DIR)/reader.c \
		$(SRC_DIR)/skeleton.c \
		$(SRC_DIR)/symtab.c \
		$(SRC_DIR)/verbose.c \
		$(SRC_DIR)/warshall.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS)
		$(CC) $(LDFLAGS) -o $(PROGRAM) $(OBJS)

clean:
		rm -f $(OBJS) $(PROGRAM)

install:	$(PROGRAM)
		cp $(PROGRAM) $(DEST)/$(PROGRAM)
		cp $(SRC_DIR)/manpage $(MAN)/yacc.1

cmp:		$(PROGRAM)
		-cmp $(PROGRAM) $(DEST)/$(PROGRAM)
		-cmp $(SRC_DIR)/manpage $(MAN)/yacc.1

pr:
		@pr proto.make $(HDRS) $(SRCS)

opr:
		make pr | opr

lint:
		$(LINT) $(LINTFLAGS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS)
		@ctags $(HDRS) $(SRCS)

closure.$(SUF):	$(SRC_DIR)/closure.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/closure.c
error.$(SUF):	$(SRC_DIR)/error.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/error.c
lalr.$(SUF):	$(SRC_DIR)/lalr.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/lalr.c
lr0.$(SUF):	$(SRC_DIR)/lr0.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/lr0.c
main.$(SUF):	$(SRC_DIR)/main.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
mkpar.$(SUF):	$(SRC_DIR)/mkpar.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/mkpar.c
output.$(SUF):	$(SRC_DIR)/output.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/output.c
reader.$(SUF):	$(SRC_DIR)/reader.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/reader.c
skeleton.$(SUF):	$(SRC_DIR)/skeleton.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/skeleton.c
symtab.$(SUF):	$(SRC_DIR)/symtab.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/symtab.c
verbose.$(SUF):	$(SRC_DIR)/verbose.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/verbose.c
warshall.$(SUF):	$(SRC_DIR)/warshall.c $(SRC_DIR)/defs.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/warshall.c
