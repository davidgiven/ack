cmake_minimum_required (VERSION 2.9)
project(em_code)

set(SRC
 bhcst.c
 bhdlb.c
 bhdnam.c
 bhfcon.c
 bhicon.c
 bhilb.c
 bhpnam.c
 bhucon.c
 crcst.c
 crdlb.c
 crdnam.c
 crxcon.c
 crilb.c
 crpnam.c
 crscon.c 
 cst.c
 dfdlb.c
 dfdnam.c
 dfilb.c
 dlb.c
 dnam.c
 end.c
 endarg.c 
 exc.c
 fcon.c
 getid.c
 icon.c
 ilb.c
 insert.c
 internerr.c 
 msend.c
 op.c
 opcst.c
 opdlb.c
 opdnam.c
 opilb.c
 opnarg.c
 oppnam.c
 pnam.c 
 pro.c
 pronarg.c
 msstart.c
 psdlb.c
 psdnam.c
 pspnam.c
 scon.c
 ucon.c 
 C_out.c
 failed.c
 em.c
 em_codeEK.h
 em_code.h
 em_codeCE.h
 em_codeO.h
 em_private.h
)

set(INCLUDE_DIRS 
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)



add_library(eme ${SRC})
target_include_directories(eme PUBLIC ${INCLUDE_DIRS})
target_compile_definitions(eme PRIVATE READABLE_EM)
target_link_libraries(eme em_data alloc print system)

add_library(emk ${SRC})
target_include_directories(emk PUBLIC ${INCLUDE_DIRS})
target_link_libraries(emk em_data alloc print system)

set_target_properties(eme PROPERTIES PUBLIC_HEADER "em_codeEK.h")
set_target_properties(emk PROPERTIES PUBLIC_HEADER "em_codeEK.h")

add_custom_command(
  OUTPUT em_codeEK.h file1.tmp
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/makeem.sed
    ${CMAKE_CURRENT_BINARY_DIR}/makeem.sed
  COMMAND sed -f makeem.sed ${CMAKE_CURRENT_SOURCE_DIR}/../../../h/em_table>file1.tmp
  COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/em.nogen file1.tmp>em_codeEK.h
  COMMENT "Regenerate em_codeEK.h from em_table"
)

install(TARGETS eme emk 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/em_code.3X DESTINATION man OPTIONAL)


