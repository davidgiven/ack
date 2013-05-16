include lang/cem/cpp.ansi/build.mk
include lang/cem/cemcom.ansi/build.mk
include lang/cem/libcc.ansi/build.mk

$(eval CCOMPILER := $(CPPANSI) $(CEMCOMANSI) $(LIBCCANSIHEADERS))

