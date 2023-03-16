set(CMAKE_C_COMPILER "/usr/bin/gcc-12")
set(CMAKE_CXX_COMPILER "/usr/bin/g++12")

set(CMAKE_CXX_FLAGS "-O2 -g3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

if(${PORT} MATCHES "sqlite3")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

