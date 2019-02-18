cmake_minimum_required (VERSION 2.9)
project(system)

set(SRC
 access.c
 break.c
 chmode.c
 close.c
 create.c
 filesize.c
 lock.c
 modtime.c
 open.c
 read.c
 remove.c
 rename.c
 seek.c
 stop.c
 system.c
 write.c
 tmpdir.c
 system.h
)

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "system.h")

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/system.3 DESTINATION man OPTIONAL)

