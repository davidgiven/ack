cmake_minimum_required (VERSION 2.9)
project (print)

set(SRC
 doprnt.c
 format.c
 fprint.c
 print.c
 sprint.c
 print.h
 param.h
)


add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "print.h")
target_link_libraries(${PROJECT_NAME} string system)

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/print.3 DESTINATION man OPTIONAL)

