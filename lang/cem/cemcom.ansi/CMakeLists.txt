cmake_minimum_required (VERSION 3.0)
project(em_cemcom.ansi)


##################################
# Location of built scripts.
##################################
set(PROJECT_SCRIPTS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../util/scripts)

set(SRC_C
	align.h
	arith.c
	arith.h
	atw.h
	blocks.c
	blocks.h
	ch3.c
	ch3.h
	ch3bin.c
	ch3bin.h
	ch3mon.c
	ch3mon.h
	class.h
	code.c
	conversion.c
	conversion.h
	cstoper.c
	cstoper.h
	dataflow.c
	dataflow.h
	declarator.c
	declarator.h
	decspecs.c
	decspecs.h
	domacro.c
	domacro.h
	dumpidf.c
	error.c
	error.h
	eval.c
	eval.h
	expr.c
	field.c
	file_info.h
	fltcstoper.c
	fltcstoper.h
	idf.c
	input.c
	input.h
	interface.h
	idf.c
	input.c
	input.h
	l_class.h
	l_comment.c
	l_comment.h
	l_em.h
	l_ev_ord.c
	l_lint.c
	l_lint.h
	l_misc.c
	l_outdef.c
	l_states.c
	label.c
	label.h
	level.h
	LLlex.c
	LLlex.h
	LLmessage.c
	main.c
	mes.h
	options.c
	options.h
	pragma.c
	pragma.h
	proto.c
	sizes.h
	skip.c
	skip.h
	specials.h
	stab.c
	stab.h
	stack.c
	stb.c
	struct.c
	switch.c
	tokenname.c
	tokenname.h
	type.c
	util.c
)

set(INCLUDE_DIRS
 .
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/../include
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/src/idf
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/src/input
)

############################################
# LLgen inputs
############################################

set(GEN_G
	${CMAKE_CURRENT_BINARY_DIR}/tokenfile.g
)



set(SRC_G
 ${CMAKE_CURRENT_SOURCE_DIR}/program.g
 ${CMAKE_CURRENT_SOURCE_DIR}/declar.g
 ${CMAKE_CURRENT_SOURCE_DIR}/expression.g
 ${CMAKE_CURRENT_SOURCE_DIR}/statement.g
 ${CMAKE_CURRENT_SOURCE_DIR}/ival.g
)


############################################
# LLgen outputs
############################################

set(GEN_SRC 
 ${CMAKE_CURRENT_BINARY_DIR}/program.c 
 ${CMAKE_CURRENT_BINARY_DIR}/declar.c 
 ${CMAKE_CURRENT_BINARY_DIR}/expression.c 
 ${CMAKE_CURRENT_BINARY_DIR}/statement.c
 ${CMAKE_CURRENT_BINARY_DIR}/ival.c
 ${CMAKE_CURRENT_BINARY_DIR}/tokenfile.c
 ${CMAKE_CURRENT_BINARY_DIR}/Lpars.c
 ${CMAKE_CURRENT_BINARY_DIR}/Lpars.h
 ${CMAKE_CURRENT_BINARY_DIR}/Lncor.c
)






set(GFILES
 ${GEN_G}
 ${SRC_G}
)



############################################
# Configuration parameters generated files
############################################

set(GEN_H_CONFIG
	${CMAKE_CURRENT_BINARY_DIR}/parameters.h
)

set(GEN_OTHERS
	${CMAKE_CURRENT_BINARY_DIR}/code.h
	${CMAKE_CURRENT_BINARY_DIR}/declar.h
	${CMAKE_CURRENT_BINARY_DIR}/def.h	
	${CMAKE_CURRENT_BINARY_DIR}/expr.h
	${CMAKE_CURRENT_BINARY_DIR}/field.h
	${CMAKE_CURRENT_BINARY_DIR}/estack.h
	${CMAKE_CURRENT_BINARY_DIR}/util.h
	${CMAKE_CURRENT_BINARY_DIR}/proto.h
	${CMAKE_CURRENT_BINARY_DIR}/replace.h
	${CMAKE_CURRENT_BINARY_DIR}/idf.h
	${CMAKE_CURRENT_BINARY_DIR}/macro.h
	${CMAKE_CURRENT_BINARY_DIR}/stack.h
	${CMAKE_CURRENT_BINARY_DIR}/stmt.h
	${CMAKE_CURRENT_BINARY_DIR}/struct.h
	${CMAKE_CURRENT_BINARY_DIR}/switch.h
	${CMAKE_CURRENT_BINARY_DIR}/type.h
	${CMAKE_CURRENT_BINARY_DIR}/l_brace.h
	${CMAKE_CURRENT_BINARY_DIR}/l_state.h
	${CMAKE_CURRENT_BINARY_DIR}/l_outdef.h
	${CMAKE_CURRENT_BINARY_DIR}/next.c
)






set(CFILES
	${SRC_C}
	${GEN_SRC}
	${GEN_H_CONFIG}
	${GEN_OTHERS}
	symbol2str.c
	char.c
)

set(LLGENOPTIONS
 -n
)

###############################################
# Generate the compile time configuration file 
###############################################
add_custom_command(
    OUTPUT ${GEN_H_CONFIG} BigPars mkparams.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mkparams.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mkparams.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/BigPars
                ${CMAKE_CURRENT_BINARY_DIR}/BigPars                      
    COMMAND sed -f ${PROJECT_SCRIPTS_SOURCE_DIR}/mkparams.sed ${CMAKE_CURRENT_BINARY_DIR}/BigPars>parameters.h
    DEPENDS BigPars)
    
###############################################
# Generate other source files
###############################################     


add_custom_command(
    OUTPUT ${GEN_OTHERS} mkalloc1.sed mkalloc2.sed mknext.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mkalloc1.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mkalloc2.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mknext.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed   
                

                
                                                    
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/code.str>code.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/declar.str>declar.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/def.str>def.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/expr.str>expr.h      
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/field.str>field.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/estack.str>estack.h      
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/util.str>util.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/proto.str>proto.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/idf.str>idf.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/macro.str>macro.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/stack.str>stack.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/stmt.str>stmt.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/struct.str>struct.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/switch.str>switch.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/type.str>type.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/l_brace.str>l_brace.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/l_state.str>l_state.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/l_outdef.str>l_outdef.h
    # Create next file from above files
    COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/next.str
                ${CMAKE_CURRENT_BINARY_DIR}/next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/code.str>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/declar.str>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/def.str>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/expr.str>>next.c  
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/field.str>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/estack.str>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/util.str>>next.c 
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/proto.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/idf.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/macro.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/stack.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/stmt.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/struct.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/switch.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/type.str>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/l_brace.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/l_state.str>>next.c      
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/l_outdef.str>>next.c      
)





add_custom_command(
	OUTPUT char.c
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../../../util/cmisc/tabgen -f${CMAKE_CURRENT_SOURCE_DIR}/char.tab >char.c
)

###############################################
# Generate the LLGen source files 
# NOTE: Order of input file is important
############################################### 

add_custom_command(
    OUTPUT tokenfile.g mktkfile.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mktkfile.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mktkfile.sed    
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mktkfile.sed ${CMAKE_CURRENT_SOURCE_DIR}/tokenname.c>tokenfile.g
)

add_custom_command(
    OUTPUT ${GEN_SRC}
    COMMAND  ${CMAKE_COMMAND} -E env LLGEN_LIB_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../../../util/LLgen/lib ${CMAKE_CURRENT_BINARY_DIR}/../../../util/LLgen/LLgen ${LLGENOPTIONS} ${GEN_G} ${SRC_G}
    DEPENDS LLgen tokenfile.g)
    
    
add_custom_command(
   OUTPUT symbol2str.c
   COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/tokcaseh.in >symbol2str.c 
   COMMAND sed -f ${CMAKE_CURRENT_SOURCE_DIR}/tokcase.sed ${CMAKE_CURRENT_SOURCE_DIR}/tokenname.c >>symbol2str.c 
   COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/tokcasee.in >>symbol2str.c 
)     
 

add_executable(${PROJECT_NAME} ${CFILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC NORCSID=1)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} emheaders emh flt alloc print string system em_data eme em_mes)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cemcom.ansi.1 DESTINATION man OPTIONAL)


