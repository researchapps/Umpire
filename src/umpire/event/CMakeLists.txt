##############################################################################
# Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and Umpire
# project contributors. See the COPYRIGHT file for details.
#
# SPDX-License-Identifier: (MIT)
##############################################################################

set (umpire_event_headers
  event.hpp
  recorder.hpp
  event_store.hpp)

set (umpire_event_sources
  recorder.cpp
  event_store.cpp)

blt_add_library(
  NAME umpire_event
  HEADERS ${umpire_event_headers}
  SOURCES ${umpire_event_sources}
  DEPENDS_ON ${umpire_event_depends}
)