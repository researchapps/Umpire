##############################################################################
# Copyright (c) 2016-21, Lawrence Livermore National Security, LLC and Umpire
# project contributors. See the COPYRIGHT file for details.
#
# SPDX-License-Identifier: (MIT)
##############################################################################

cmake_policy(SET CMP0025 NEW)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0057 NEW)
cmake_policy(SET CMP0054 NEW)

include(CMakeDependentOption)
include(CMakePackageConfigHelpers)

project(Umpire
  LANGUAGES CXX C
  VERSION 6.0.0)

cmake_minimum_required(VERSION 3.9)

set(UMPIRE_VERSION_RC "")

include(cmake/SetupUmpireOptions.cmake)

if (UMPIRE_ENABLE_SYCL)
  set(BLT_CXX_STD "c++17" CACHE STRING "Version of C++ standard")
   if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl")
   endif()
else()
  set(BLT_CXX_STD "c++11" CACHE STRING "Version of C++ standard")
endif()
set(CMAKE_CUDA_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "PGI")
  message(STATUS "Setting CMAKE_CXX_EXTENSIONS to ON for PGI Compiler")
  set( CMAKE_CXX_EXTENSIONS ON )
endif()

message(STATUS "Using CMake version ${CMAKE_VERSION}")

if (UMPIRE_ENABLE_NUMA AND (NOT UNIX OR APPLE))
  message(FATAL_ERROR "\
  NUMA support unavailable. \
  Please re-configure with UMPIRE_ENABLE_NUMA=Off (default value)")
endif ()

set(BLT_EXPORT_THIRDPARTY ON CACHE BOOL "")

################################
# BLT
################################
if (NOT BLT_LOADED)
  if (DEFINED BLT_SOURCE_DIR)
    if (NOT EXISTS ${BLT_SOURCE_DIR}/SetupBLT.cmake)
        message(FATAL_ERROR "Given BLT_SOURCE_DIR does not contain SetupBLT.cmake")
    endif()
  else ()
    set (BLT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/blt CACHE PATH "")

    if (NOT EXISTS ${BLT_SOURCE_DIR}/SetupBLT.cmake)
      message(FATAL_ERROR
          "The BLT git submodule is not present. "
          "Either run the following two commands in your git repository: \n"
          "    git submodule init\n"
          "    git submodule update\n"
          "Or add -DBLT_SOURCE_DIR=/path/to/blt to your CMake command." )
    endif ()
  endif ()

  include(${BLT_SOURCE_DIR}/SetupBLT.cmake)
endif()

#######################################
# Options that depend on BLT Options
#######################################
cmake_dependent_option( UMPIRE_ENABLE_CUDA "Build Umpire with CUDA support" On
                        "ENABLE_CUDA" Off )
cmake_dependent_option( UMPIRE_ENABLE_HIP "Build Umpire with HIP" On
                       "ENABLE_HIP" Off )
cmake_dependent_option( UMPIRE_ENABLE_OPENMP "Build Umpire with OpenMP" On
                       "ENABLE_OPENMP" Off )
cmake_dependent_option( UMPIRE_ENABLE_MPI "Build Umpire with MPI" On
                       "ENABLE_MPI" Off )
cmake_dependent_option( ENABLE_IPC_SHARED_MEMORY "Enable Host Shared Memory Resource" ${ENABLE_MPI}
                       "NOT WIN32;NOT APPLE" Off )


cmake_dependent_option( UMPIRE_ENABLE_TESTS "Build Umpire tests" On
                       "ENABLE_TESTS" Off )
cmake_dependent_option( UMPIRE_ENABLE_BENCHMARKS "Build Umpire benchmarks" On
                        "ENABLE_BENCHMARKS" Off )
cmake_dependent_option( UMPIRE_ENABLE_EXAMPLES "Build Umpire examples" On
                        "ENABLE_EXAMPLES" Off )

cmake_dependent_option( UMPIRE_ENABLE_DOCS "Build Umpire docs" On
                        "ENABLE_DOCS" Off )
cmake_dependent_option( UMPIRE_ENABLE_CLANGQUERY "Build Umpire with Clang query" On
                        "ENABLE_CLANGQUERY" Off )
cmake_dependent_option( UMPIRE_ENABLE_COVERAGE "Build Umpire with Coverage support (with GCC)" On
                        "ENABLE_COVERAGE" Off )
cmake_dependent_option( UMPIRE_ENABLE_GMOCK "Build Umpire with gmock" On
                        "ENABLE_GMOCK" Off )
cmake_dependent_option( UMPIRE_ENABLE_FORTRAN "Build Umpire with FORTRAN" On
                        "ENABLE_FORTRAN" Off )

if (UMPIRE_ENABLE_FORTRAN)
  set(UMPIRE_ENABLE_C On)
endif()

if (UMPIRE_ENABLE_DOCS AND NOT ENABLE_DOXYGEN)
  message(FATAL_ERROR "\
  Sphinx documentation requires Doxygen, \
  please re-configure with ENABLE_DOXYGEN=ON")
endif ()

if (Git_FOUND)
  blt_git_hashcode (HASHCODE umpire_sha1
                    RETURN_CODE rc
                    SOURCE_DIR ${PROJECT_SOURCE_DIR})
  set (UMPIRE_VERSION_RC ${umpire_sha1})
endif ()

include(cmake/SetupCMakeBasics.cmake)
include(cmake/SetupCompilerFlags.cmake)
include(cmake/SetupUmpireThirdParty.cmake)

add_subdirectory(src)

configure_file(
  umpire-config.cmake.in
  "${PROJECT_BINARY_DIR}/umpire-config.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/umpire-config.cmake"
  DESTINATION share/umpire/cmake)

write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/umpire-config-version.cmake
  COMPATIBILITY SameMajorVersion)

install(FILES 
  "${PROJECT_BINARY_DIR}/umpire-config-version.cmake"
  DESTINATION share/umpire/cmake)

install(EXPORT umpire-targets DESTINATION share/umpire/cmake)

if (UMPIRE_ENABLE_TESTS)
  add_subdirectory(tests)
endif ()

if (UMPIRE_ENABLE_DEVELOPER_BENCHMARKS)
  add_subdirectory(benchmarks)
  if ((NOT CMAKE_BUILD_TYPE) OR (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release"))
    message("-- Warning: CMAKE_BUILD_TYPE not set to Release, benchmark information will not be reliable for this build!")
  endif()
else()
  if (UMPIRE_ENABLE_BENCHMARKS)
    message("-- Warning: Benchmarks will not be built. If you want to build with benchmarks,\n"
          "     set UMPIRE_ENABLE_DEVELOPER_BENCHMARKS to On.")
  endif()
endif ()

if (UMPIRE_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif ()

if (UMPIRE_ENABLE_TOOLS)
  add_subdirectory(tools)
endif ()

if (UMPIRE_ENABLE_DOCS)
  add_subdirectory(docs)
endif ()
