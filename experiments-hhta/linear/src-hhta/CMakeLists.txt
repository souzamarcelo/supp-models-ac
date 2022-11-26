cmake_minimum_required(VERSION 2.8)

project(aac-bqp)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-g -ggdb -Wall -Wno-unknown-pragmas -fPIC -DIL_STD -fexceptions -std=c++0x -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native -openmp -O3 -DNDEBUG")

## if you want to use the standard Boost (often in /usr/local/include): set this to OFF
set(Boost_NO_SYSTEM_PATHS OFF)
#set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options timer system chrono filesystem)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

include_directories("${PROJECT_BINARY_DIR}")
add_executable(grammar grammar.cpp)
add_executable(runner runner.cpp)

#For Linux
#target_link_libraries(grammar ${Boost_LIBRARIES} rt)
#target_link_libraries(runner ${Boost_LIBRARIES} rt)

#For Mac OS X
target_link_libraries(grammar ${Boost_LIBRARIES})
target_link_libraries(runner ${Boost_LIBRARIES})
