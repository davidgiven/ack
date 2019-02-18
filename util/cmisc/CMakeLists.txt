cmake_minimum_required (VERSION 2.9)
project (cmisc)


add_executable(tabgen tabgen.c)
target_compile_definitions(tabgen PUBLIC NORCSID=1)

add_executable(ed ed.c)
target_compile_definitions(ed PUBLIC NORCSID=1)


install(TARGETS tabgen ed DESTINATION bin)
install(FILES 
 ${CMAKE_CURRENT_SOURCE_DIR}/tabgen.1
 ${CMAKE_CURRENT_SOURCE_DIR}/ed.1
 DESTINATION man OPTIONAL)

