cmake_minimum_required (VERSION 2.9)
project(em_data)

set(SRC
 em_spec.h
 em_pseu.h
 em_pseu.c
 em_mnem.h
 em_mnem.c
 em_flag.c
 em_ptyp.c
)

set(INCLUDE_DIRS 
 ${CMAKE_CURRENT_SOURCE_DIR}/../../h 
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../h
)



add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} emheaders emh)

add_executable(makeem makeem.c)
target_link_libraries(makeem data)

add_custom_command(
  OUTPUT em_spec.h em_pseu.h em_pseu.c em_mnem.h em_mnem.c em_flag.c
  COMMAND makeem  ${CMAKE_CURRENT_SOURCE_DIR}/../../../h/em_table
  COMMENT "Generating em_data files"
  DEPENDS makeem
)

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)




