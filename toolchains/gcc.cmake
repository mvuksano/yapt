find_program(GCC_COMPILER gcc gcc-12)

if(GCC_COMPILER)
    set(CMAKE_C_COMPILER ${GCC_COMPILER})
else()
    message(FATAL_ERROR "gcc compiler not found")
endif()

find_program(GPP_COMPILER g++ g++-12)
if(GPP_COMPILER)
    set(CMAKE_CXX_COMPILER ${GPP_COMPILER})
else()
    message(FATAL_ERROR "g++ compiler not found")
endif()

set(CMAKE_CXX_FLAGS "-O2 -g3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

if(${PORT} MATCHES "sqlite3")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()
