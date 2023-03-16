set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

if(${PORT} MATCHES "sqlite3")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../toolchains/gcc.cmake)
