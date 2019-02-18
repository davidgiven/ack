cmake_minimum_required (VERSION 3.0)

enable_testing()

# Watch out on the order of the building, some modules and libraries 
# must be built before others.

# if (MSVC)
#	add_compile_options(/W3 /WX)
# else()
# 	add_compile_options(-W -Wall -Werror)
# endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/h)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/h)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/alloc)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/string)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/print)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/flt_arith)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/data)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/system)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/em_data)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/em_code)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/em_mes)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/object)

# Incomplete, as it should regenerate the em_tables.
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/read_em)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/make)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/arch)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/amisc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/cmisc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/LLgen)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/misc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/topgen)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/cpp)

# Compilers and runtime libraries
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/basic/src)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/pc/comp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/cem/cemcom.ansi)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/cem/cpp.ansi)


# Should probably not be present, assuming it is installed on target.
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/byacc)

# Compilers / assemblers / linkers
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6500/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6800/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6805/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6809/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/arm/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/i386/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/i80/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/i86/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/m68020/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/m68k2/as)
# Currently unported.
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/mips/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/ns/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/pdp/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/powerpc/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/s2650/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/vax4/as)
# Does not build on current CLang 8.0
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/vc4/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/z80/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/z8000/as)

# Old code generators
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/cgg)

# CPU Specific coge generators
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6500/cg)



