cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Sigma0)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS}")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()
  
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net EG)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

set(SRCS
  FileReader.cxx
  Fitter.cxx
  Plotter.cxx
   )

 set(HEADERS
   FileReader.h
   Fitter.h
   Plotter.h
   )

ROOT_GENERATE_DICTIONARY(G__dict ${HEADERS} LINKDEF LinkDef.h)
add_library(dict SHARED ${SRCS} G__dict)
target_link_libraries(dict ${ROOT_LIBRARIES})

add_executable(lambdaSpectrum Scripts/lambdaSpectrum.C)
target_link_libraries(lambdaSpectrum dict)

add_executable(sigmaSpectrum Scripts/sigmaSpectrum.C)
target_link_libraries(sigmaSpectrum dict)

add_executable(plotQA Scripts/plotQA.C)
target_link_libraries(plotQA dict)
