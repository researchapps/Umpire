//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and Umpire
// project contributors. See the COPYRIGHT file for details.
//
// SPDX-License-Identifier: (MIT)
//////////////////////////////////////////////////////////////////////////////
#ifndef UMPIRE_recorder_HPP
#define UMPIRE_recorder_HPP

#include "umpire/event/event.hpp"

#include <fstream>

namespace umpire {
namespace event {

class file_recorder {
  public:

  void record(event e);

  private:
  static std::ofstream m_fstream;
};

}
}
#endif