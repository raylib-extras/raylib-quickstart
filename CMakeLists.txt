cmake_minimum_required(VERSION 3.25)
set(CMAKE_C_STANDARD 23)    # you can change this to whatever you want

project(RaylibQuickstart)

if(${PROJECT_NAME} STREQUAL "RaylibQuickstart")
	message(WARNING "Please change the project name before you continue working.")
endif()

message(STATUS "Looking for ccache/sccache")
find_program(CCACHE_PROGRAM ccache)
find_program(SCCACHE_PROGRAM sccache)
if(CCACHE_PROGRAM)
	message(NOTICE "Using ccache: ${CCACHE_PROGRAM}")
	set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
	set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
elseif(SCCACHE_PROGRAM)
	message(NOTICE "Using sccache: ${SCCACHE_PROGRAM}")
	set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_PROGRAM})
	set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_PROGRAM})
else()
	message(NOTICE "Not using a caching compiler."
	"It is recommended to use one to improve build times.")
endif()

if(DEFINED ENV{CPM_SOURCE_CACHE})
	message(NOTICE "Using CPM_SOURCE_CACHE: $ENV{CPM_SOURCE_CACHE}")
else()
	message(NOTICE "Not using CPM_SOURCE_CACHE."
	"It is recommended to add it to your environment to cache dependency downloads.")
endif()

include(cmake/CPM.cmake)
CPMAddPackage("gh:raysan5/raylib#master")

file(GLOB SOURCES CONFIGURE_DEPENDS
	src/*.c
	# add additional folders here to compile all of their contents
)

if(WIN32)
	set(RESOURCE_FILE src/application.rc)
else()
	set(RESOURCE_FILE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCE_FILE})

target_include_directories(${PROJECT_NAME} PRIVATE 
	${raylib_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} raylib)
