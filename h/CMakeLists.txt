cmake_minimum_required (VERSION 3.0)
project(emheaders)

# Create an include only library to be used
# for easier dependency management.

# Created when generating the CMake file.
file(WRITE  ${CMAKE_CURRENT_BINARY_DIR}/local.h "#define VERSION 3")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/local.h "#define ACKM ")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/local.h "#define BIGMACHINE 1")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/local.h "#define SYS_5")


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})


install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)



