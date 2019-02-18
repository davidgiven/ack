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
 h.h
)

# The following should be added if utime.h and unistd.h are
# available.

# add_definitions(-Dunix)

add_executable(${PROJECT_NAME} ${SRC})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

