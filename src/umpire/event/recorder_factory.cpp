//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and Umpire
// project contributors. See the COPYRIGHT file for details.
//
// SPDX-License-Identifier: (MIT)
//////////////////////////////////////////////////////////////////////////////

#include "umpire/event/recorder_factory.hpp"

#if !defined(_MSC_VER)
#include <unistd.h> // getpid()
#else
#include <process.h>
#define getpid _getpid
#include <direct.h>
#endif

namespace umpire {
namespace event {

file_recorder&
recorder_factory::get_recorder()
{
  static const std::string filename{"umpire." + std::to_string(getpid()) + ".stats"};
  static file_recorder recorder{filename};

  return recorder;
}

}
}