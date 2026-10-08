
cmake_minimum_required (VERSION 3.8)

project ("CoveringRadiusCosets" LANGUAGES CXX) 

if(NOT CMAKE_CONFIGURATION_TYPES)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "")
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build Conf" FORCE)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()	

add_executable (CoveringRadiusCosets "CoveringRadiusCosets.cpp")


find_package(OpenMP REQUIRED)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

find_package(MPI REQUIRED) 
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories(${MPI_INCLUDE_PATH})

target_link_libraries(CoveringRadiusCosets ${MPI_LIBRARIES})


