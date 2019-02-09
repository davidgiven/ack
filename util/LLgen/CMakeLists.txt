cmake_minimum_required (VERSION 2.9)
project(LLgen)

set(SRC
 src/main.c
 src/gencode.c
 src/compute.c
 src/LLgen.c
 src/tokens.c
 src/check.c
 src/reach.c
 src/global.c
 src/name.c
 src/sets.c
 src/Lpars.c
 src/alloc.c
 src/machdep.c
 src/cclass.c
 src/savegram.c
 src/cclass.h
 src/extern.h
 src/io.h
 src/Lpars.h
 src/sets.h
 src/types.h
)

set(INCLUDE_DIRS src)

include_directories(${INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC})
target_compile_definitions(${PROJECT_NAME} PUBLIC NDEBUG=1 NON_CORRECTING=1 NORCSID=1)

file(COPY lib DESTINATION .)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

