########################################################################
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.9.0)
PROJECT(item_server CXX)
ENABLE_TESTING()

if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

set(CMAKE_MODULE_PATH APPEND "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_STANDARD 11)

if (UNIX)
	ADD_DEFINITIONS(-fPIC)
	ADD_DEFINITIONS(-fpermissive)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath-link=${USER_RPATH}")

endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)

link_directories(${PROJECT_BINARY_DIR}/libs) 


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/include)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/test)

MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
