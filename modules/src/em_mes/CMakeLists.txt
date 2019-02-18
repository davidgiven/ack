cmake_minimum_required (VERSION 3.0)
project(em_mes)


set(SRC_C
 C_ms_com.c
 C_ms_ego.c
 C_ms_emx.c
 C_ms_err.c
 C_ms_flt.c
 C_ms_gto.c
 C_ms_opt.c
 C_ms_par.c
 C_ms_reg.c
 C_ms_src.c
 C_ms_stb.c
 C_ms_std.c
)

set(INCLUDE_DIRS
 .
)


add_library(${PROJECT_NAME} ${SRC_C})
target_compile_definitions(${PROJECT_NAME} PUBLIC NORCSID=1)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} emheaders emh emk em_data)

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/em_mes.3 DESTINATION man OPTIONAL)
