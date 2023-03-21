find_program(GCC_COMPILER NAMES gcc-12 gcc)

if(GCC_COMPILER)
    set(CMAKE_C_COMPILER ${GCC_COMPILER})
else()
    message(FATAL_ERROR "gcc compiler not found")
endif()

find_program(GPP_COMPILER NAMES g++-12 g++)
if(GPP_COMPILER)
    set(CMAKE_CXX_COMPILER ${GPP_COMPILER})
else()
    message(FATAL_ERROR "g++ compiler not found")
endif()

# To see generated templates add -save-temps=obj flag.
set(CMAKE_CXX_FLAGS "-O2 -g3")
set(CMAKE_CXX_STANDARD 20)

find_program(MOLD_LINKER mold)
if(MOLD_LINKER)
    set(CMAKE_CXX_FLAGS "-fuse-ld=mold")
endif()
set(CMAKE_BUILD_TYPE RelWithDebInfo)

if(${PORT} MATCHES "sqlite3")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()
