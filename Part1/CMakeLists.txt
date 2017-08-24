project (wave)
site_name (www.ostis.net)
cmake_minimum_required (VERSION 2.6)

set(SC_MACHINE_ROOT "${CMAKE_SOURCE_DIR}/../sc-machine/")
set(SCP_MACHINE_ROOT "${CMAKE_SOURCE_DIR}")

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin)

link_directories(${SC_MACHINE_ROOT}/bin)

set(SC_MEMORY_SRC "${SC_MACHINE_ROOT}/sc-memory/")

# find dependencies
if (${UNIX})
	include(FindPkgConfig)
	pkg_check_modules (GLIB2 REQUIRED glib-2.0)
	pkg_check_modules (GLIB2_MODULE REQUIRED gmodule-2.0)

	set (GLIB2_INCLUDE_DIRS "${GLIB2_INCLUDE_DIRS}" "/usr/lib/x86_64-linux-gnu/glib-2.0/include/" "${GLIB2_MODULE}")
	set (GLIB2_LIBRARIES "${GLIB2_LIBRARIES}" "${GLIB2_MODULE_LIBRARIES}")

endif(${UNIX})

if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	if(MSVC)
		message(STATUS "Compiler: MSVC, version: " ${MSVC_VERSION})
	endif(MSVC)
	if(BORLAND) 
		message(STATUS "Compiler: BCC") 
	endif(BORLAND) 
	else(${CMAKE_SYSTEM_NAME} STREQUAL Linux) 
		message(STATUS "Only GCC is supported on Linux")
	endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows) 

message(STATUS "CMake generates " ${CMAKE_GENERATOR})

add_executable(wave wavefindpath.cpp utils.cpp utils.h)
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

target_link_libraries(wave sc-memory ${GLIB2_LIBRARIES})
