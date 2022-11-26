cmake_minimum_required(VERSION 2.8)

project(bqp-library)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-g -ggdb -Wall -Wno-unknown-pragmas -fPIC -DIL_STD -fexceptions -std=c++0x -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native -openmp -O3 -DNDEBUG")

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS ON)
set(BOOST_INCLUDEDIR /usr/local/include)
include_directories(SYSTEM ${BOOST_INCLUDEDIR})
find_package(Boost COMPONENTS program_options timer system chrono filesystem)

add_executable(bqpsolve bqpsolve.cpp bqp.cpp rng.cpp)
target_link_libraries(bqpsolve ${Boost_LIBRARIES} ${CPLEX_LIBRARIES})

add_executable(bqpinfo bqpinfo.cpp bqp.cpp rng.cpp)
target_link_libraries(bqpinfo ${Boost_LIBRARIES} ${CPLEX_LIBRARIES})