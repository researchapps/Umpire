//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and Umpire
// project contributors. See the COPYRIGHT file for details.
//
// SPDX-License-Identifier: (MIT)
//////////////////////////////////////////////////////////////////////////////

#include "umpire/event/file_recorder.hpp"

#include "umpire/tpl/json/json.hpp"

#if !defined(_MSC_VER)
#include <unistd.h> // getpid()
#else
#include <process.h>
#define getpid _getpid
#include <direct.h>
#endif

namespace umpire {
namespace event {

std::ofstream file_recorder::m_fstream{"umpire." + std::to_string(getpid()) + ".stats"};

void 
file_recorder::record(event e) {
  nlohmann::json je;

  je["name"] = e.m_name;
  // TODO: only handles the two event types
  je["category"] = (e.m_category == category::operation ) ? "operation" : "statistic";
  for (const auto& it : e.m_int_args ) {
    std::string name;
    int value;
    std::tie(name, value) = it;
    je["args"][name] = value;
  }

  for (const auto& it : e.m_string_args ) {
    std::string name;
    std::string value;
    std::tie(name, value) = it;
    je["args"][name] = value;
  }

  for (const auto& tag : e.m_tags) {
    je["tags"] += tag;
  }

  je["timestamp"] = std::to_string(static_cast<long>(std::chrono::time_point_cast<std::chrono::nanoseconds>(e.m_timestamp).time_since_epoch().count()));

  m_fstream << je << std::endl;
}

}
}