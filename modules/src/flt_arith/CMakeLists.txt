cmake_minimum_required (VERSION 2.9)
project (flt)

enable_testing()

set(SRCS
 b64_add.c
 b64_sft.c
 flt_add.c
 flt_ar2flt.c
 flt_arith.h
 flt_chk.c
 flt_cmp.c
 flt_div.c
 flt_flt2ar.c
 flt_misc.h
 flt_modf.c
 flt_mul.c
 flt_nrm.c
 flt_str2fl.c
 flt_umin.c
 split.c
)

add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ../../h ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "flt_arith.h")

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/flt_arith.3 DESTINATION man OPTIONAL)

#############################################
# Test targets 
#############################################
add_executable(testflt test.c)
target_link_libraries(testflt ${PROJECT_NAME})

add_test(NAME ops_testflt COMMAND testflt)

# This is a bug workaround with CMake
add_custom_target(tests COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS testflt)