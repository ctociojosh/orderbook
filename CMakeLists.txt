cmake_minimum_required(VERSION 3.8)
project(cs-orderbook)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CXX_FLAGS
        -std=c++1y
        )

#set(Boost_NO_SYSTEM_PATHS TRUE)
#
#if (Boost_NO_SYSTEM_PATHS)
#    if( WIN32 )
#        set(BOOST_ROOT "C:\\devtool\\boost_1_62")
#        set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib64-msvc-14.0")
#        set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/")
#    elseif (APPLE)
#        set(BOOST_ROOT "/Users/zhour/devtool/boost")
#        set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
#        set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
#    endif()
#
#
#    message("use boost lib" ${BOOST_LIBRARY_DIRS}, ${BOOST_INCLUDE_DIRS})
#endif (Boost_NO_SYSTEM_PATHS)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost COMPONENTS system filesystem thread-mt log log_setup REQUIRED)

include_directories(${BOOST_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})

message("project path:" ${PROJECT_SOURCE_DIR})
message("boost po library:" ${BOOSTPO_LIBRARY})

set(SOURCE_FILES
        main.cpp
        main.cpp engine.cpp feed_handler.cpp error_monitor.cpp )

add_executable(matchengine ${SOURCE_FILES})
target_link_libraries(matchengine ${Boost_LIBRARIES})
