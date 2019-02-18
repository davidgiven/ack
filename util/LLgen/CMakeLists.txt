cmake_minimum_required (VERSION 2.9)
project(LLgen)

enable_testing()

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

# Correcting version.
add_executable(llgen_c ${SRC})
target_compile_definitions(llgen_c PUBLIC NORCSID=1)

file(COPY lib DESTINATION .)

#############################################
# Regenerate the actual grammar files
#############################################
add_custom_target(regenerate
 COMMAND 
   ${CMAKE_COMMAND} -E copy 
     ${CMAKE_CURRENT_SOURCE_DIR}/src/tokens.g 
     ${CMAKE_CURRENT_BINARY_DIR}/tokens.g
 COMMAND
   ${CMAKE_COMMAND} -E copy 
     ${CMAKE_CURRENT_SOURCE_DIR}/src/Llgen.g 
     ${CMAKE_CURRENT_BINARY_DIR}/Llgen.g
 COMMAND ${PROJECT_NAME} -vvv tokens.g LLgen.g  
 COMMAND 
   ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_BINARY_DIR}/Llgen.c ${CMAKE_CURRENT_SOURCE_DIR}/src/LLgen.c
 COMMAND 
   ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_BINARY_DIR}/tokens.c ${CMAKE_CURRENT_SOURCE_DIR}/src/tokens.c 
 COMMAND 
   ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_BINARY_DIR}/Lpars.c ${CMAKE_CURRENT_SOURCE_DIR}/src/Lpars.c
 COMMAND 
   ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_BINARY_DIR}/Lpars.h ${CMAKE_CURRENT_SOURCE_DIR}/src/Lpars.h               
 COMMENT "Regenerate grammar files and copy them to source"
)
add_dependencies(regenerate ${PROJECT_NAME})




#############################################
# Test targets 
#############################################

# Try to rebuild the own LLgen grammar.
add_test(NAME llgentest COMMAND ${PROJECT_NAME} -vvv 
 ${CMAKE_CURRENT_SOURCE_DIR}/src/tokens.g 
 ${CMAKE_CURRENT_SOURCE_DIR}/src/LLgen.g)
 
############################################
# Install targets
#############################################

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/LLgen.1 DESTINATION man OPTIONAL)
install(FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/incl
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/nc_incl
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/nc_rec
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/rec
  DESTINATION lib)
 