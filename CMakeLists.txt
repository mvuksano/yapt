cmake_minimum_required(VERSION 3.12)
project(yapt)

set(EVENTLIB_DEBUG_ENABLE True)

find_package(Boost REQUIRED COMPONENTS thread)
find_package(GTest CONFIG REQUIRED)
find_package(Libevent CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

find_library(JEMALLOC_LIB jemalloc REQUIRED)

configure_file(build_options.h.in build_options.h)

set(SOURCES application.h application.cpp utils.hpp icmp.h eventbase.h eventbase.cpp event.h event.cpp context.h context.cpp grpc_server.h grpc_server.cpp protos/service.pb.h protos/service.pb.cc protos/service.grpc.pb.h protos/service.grpc.pb.cc)
set(LIBEVENT_LIBS libevent::core libevent::extra libevent::pthreads)

add_executable(yapt main.cpp ${SOURCES})
target_link_libraries(yapt PUBLIC ${JEMALLOC_LIB} ${Boost_LIBRARIES} glog::glog gflags::gflags ${LIBEVENT_LIBS} gRPC::grpc++)
target_include_directories(yapt PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${Boost_INCLUDE_DIRS} ${gRPC_INCLUDE_DIRS}) 
target_link_options(yapt PUBLIC "-static-libstdc++")

add_executable(yapt_test main_test.cpp ${SOURCES})
target_include_directories(yapt PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${Boost_INCLUDE_DIRS} ${gRPC_INCLUDE_DIRS})  
target_link_libraries(yapt_test PRIVATE ${JEMALLOC_LIB} GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main glog::glog gflags::gflags ${LIBEVENT_LIBS} gRPC::grpc++)

enable_testing()
add_test(NAME yapt WORKING_DIRECTORY ${CMAKE_BUILD_DIR} COMMAND yapt_test)
