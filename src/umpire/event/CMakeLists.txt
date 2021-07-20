##############################################################################
# Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and Umpire
# project contributors. See the COPYRIGHT file for details.
#
# SPDX-License-Identifier: (MIT)
##############################################################################

set (umpire_event_headers
  event.hpp
  file_recorder.hpp
  recorder_factory.hpp)

set (umpire_event_sources
  file_recorder.cpp
  recorder_factory.cpp)

set (umpire_event_depends umpire_tpl_json)

blt_add_library(
  NAME umpire_event
  HEADERS ${umpire_event_headers}
  SOURCES ${umpire_event_sources}
  DEPENDS_ON ${umpire_event_depends}
)

target_include_directories(
  umpire_event
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>)

install(TARGETS
  umpire_event
  EXPORT umpire-targets
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES
  ${umpire_event_headers}
  DESTINATION include/umpire/event)