cmake_minimum_required(VERSION 3.12)
project(yapt)

set(EVENTLIB_DEBUG_ENABLE True)

find_package(Boost REQUIRED COMPONENTS thread)
find_package(GTest CONFIG REQUIRED)
find_package(Libevent CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)

configure_file(build_options.h.in build_options.h)

set(SOURCES application.h application.cpp utils.hpp icmp.h eventbase.h eventbase.cpp event.h event.cpp context.h context.cpp)
set(LIBEVENT_LIBS libevent::core libevent::extra libevent::pthreads)

add_executable(yapt main.cpp ${SOURCES})
target_link_libraries(yapt PUBLIC ${Boost_LIBRARIES} glog::glog gflags::gflags ${LIBEVENT_LIBS})
target_include_directories(yapt PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${Boost_INCLUDE_DIRS})

add_executable(yapt_test main_test.cpp ${SOURCES})
target_link_libraries(yapt_test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main glog::glog gflags::gflags ${LIBEVENT_LIBS})

enable_testing()
add_test(NAME yapt WORKING_DIRECTORY ${CMAKE_BUILD_DIR} COMMAND yapt_test)
