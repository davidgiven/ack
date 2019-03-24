cmake_minimum_required (VERSION 2.9)
project (ack-misc)

set(SRC_MODULES abmodules.c)
set(SRC_ELFLOD aelflod.c)
set(SRC_ANM anm.c)
set(SRC_ASHOW ashow.c)
set(SRC_ASIZE asize.c)
set(SRC_ASLOD aslod.c)
set(SRC_ASTRIP astrip.c)




add_executable(anm ${SRC_ANM})
add_executable(ashow ${SRC_ASHOW})
add_executable(asize ${SRC_ASIZE})
add_executable(astrip ${SRC_ASTRIP})
add_executable(aslod ${SRC_ASLOD})


include_directories(${INCLUDE_DIRS})

# Set linking options
#target_link_libraries(ack-modules emheaders object)
target_link_libraries(anm emheaders object)
target_link_libraries(ashow emheaders object)
target_link_libraries(asize emheaders object)
target_link_libraries(astrip emheaders object system)
target_link_libraries(aslod emheaders object)

install(TARGETS anm DESTINATION bin)
install(TARGETS ashow DESTINATION bin)
install(TARGETS asize DESTINATION bin)
install(TARGETS astrip DESTINATION bin)
install(TARGETS aslod DESTINATION bin)