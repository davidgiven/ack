cmake_minimum_required (VERSION 3.2)
project(em_topgen)
set(SRC
 main.c
 topgen.c
 Lpars.c
 LLlex.c
 symtab.c
 symtab.h
 pattern.c
 hash.c
 token.h
 tunable.h
)
set(INCLUDE_DIRS
 .
 ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(
    OUTPUT topgen.c Lpars.c Lpars.h
    COMMAND  ${CMAKE_COMMAND} -E env LLGEN_LIB_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../LLgen/lib ${CMAKE_CURRENT_BINARY_DIR}/../LLgen/LLgen "${CMAKE_CURRENT_SOURCE_DIR}/topgen.g"
 #   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../LLgen/llgen "${CMAKE_CURRENT_SOURCE_DIR}/topgen.g"
    MAIN_DEPENDENCY topgen.g
    DEPENDS LLgen)
add_executable(topgen ${SRC})
target_include_directories(topgen PRIVATE ${INCLUDE_DIRS})
target_link_libraries(topgen)
install(TARGETS topgen DESTINATION bin)
