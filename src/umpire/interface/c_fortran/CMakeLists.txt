##############################################################################
# Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and Umpire
# project contributors. See the COPYRIGHT file for details.
#
# SPDX-License-Identifier: (MIT)
##############################################################################
set (umpire_interface_c_fortran_headers
  wrapAllocator.h
  wrapResourceManager.h
  wrapUmpire.h
  umpire.h
  typesUmpire.h)

set (umpire_interface_c_fortran_sources
  utilUmpire.cpp
  wrapAllocator.cpp
  wrapResourceManager.cpp
  wrapUmpire.cpp)

if (UMPIRE_ENABLE_FORTRAN)
  set (umpire_interface_c_fortran_sources
    ${umpire_interface_c_fortran_sources}
    wrapfumpire.f)

  set_source_files_properties(
    wrapfumpire.f
    PROPERTIES
    Fortran_FORMAT FREE)
endif ()
    
set_source_files_properties(
  wrapResourceManager.cpp 
  PROPERTIES 
  COMPILE_FLAGS ${UMPIRE_DISABLE_DEPRECATED_WARNINGS_FLAG})

set(umpire_interface_c_fortran_depends camp)
blt_list_append( TO umpire_interface_c_fortran_depends ELEMENTS cuda_runtime IF UMPIRE_ENABLE_CUDA )
blt_list_append( TO umpire_interface_c_fortran_depends ELEMENTS hip_runtime IF UMPIRE_ENABLE_HIP )
blt_list_append( TO umpire_interface_c_fortran_depends ELEMENTS mpi IF UMPIRE_ENABLE_MPI )

blt_add_library(
  NAME umpire_interface
  HEADERS ${umpire_interface_c_fortran_headers}
  SOURCES ${umpire_interface_c_fortran_sources}
  DEPENDS_ON ${umpire_interface_c_fortran_depends}
  OBJECT TRUE)

target_include_directories(
  umpire_interface
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>)

install(FILES
  ${umpire_interface_c_fortran_headers}
  DESTINATION include/umpire/interface/c_fortran)

install(DIRECTORY
  ${CMAKE_Fortran_MODULE_DIRECTORY}/
  DESTINATION include/umpire
  FILES_MATCHING PATTERN "*.mod")
