cmake_minimum_required (VERSION 2.9)
project (string)

set(SRC
 bts2str.c
 btscat.c
 btscmp.c
 btscpy.c
 btszero.c
 long2str.c
 str2bts.c
 str2long.c
 strindex.c
 strrindex.c
 strzero.c
 ack_string.h
)


add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "ack_string.h")

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/string.3 DESTINATION man OPTIONAL)

