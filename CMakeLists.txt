cmake_minimum_required(VERSION 3.12)
project(yapt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_C_COMPILER /usr/bin/clang)

# set(CMAKE_CXX_FLAGS -fuse-ld=mold)

find_package(Boost REQUIRED COMPONENTS thread)
find_package(GTest CONFIG REQUIRED)
find_package(Libevent CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)


add_executable(main main.cpp application.h application.cpp icmp.h eventbase.h eventbase.cpp event.h event.cpp context.h context.cpp)
target_link_libraries(main PUBLIC ${Boost_LIBRARIES} glog::glog gflags::gflags libevent::core libevent::extra libevent::pthreads)
target_include_directories(main PUBLIC ${Boost_INCLUDE_DIRS})

# add_executable(main_test main_test.cpp)
# target_link_libraries(main_test PRIVATE GTest::gtest GTest::gtest_main)

enable_testing()
# add_test(NAME main WORKING_DIRECTORY ${CMAKE_BUILD_DIR}/bin COMMAND main_test)
