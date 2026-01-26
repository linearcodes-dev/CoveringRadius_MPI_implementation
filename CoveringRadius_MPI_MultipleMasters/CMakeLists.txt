
cmake_minimum_required (VERSION 3.8)

project ("CoveringRadiusMPI") 

add_executable (CoveringRadiusMPI "main.cpp" "Covering_functions.h" "Data.h" "LinearCombinations.h" "Polynomials.h")

find_package(MPI REQUIRED) 
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native ")
endif()	

include_directories(${MPI_INCLUDE_PATH})

target_link_libraries(CoveringRadiusMPI ${MPI_LIBRARIES})

