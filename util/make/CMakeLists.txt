cmake_minimum_required (VERSION 2.6)
project (ack-make)

set(SRC 
 check.c
 input.c
 macro.c
 make.c
 main.c
 reader.c
 rules.c
)

# The following should be added if utime.h and unistd.h are
# available.

# add_definitions(-Dunix)

add_executable(ack-make ${SRC})

install(TARGETS ack-make DESTINATION bin)
