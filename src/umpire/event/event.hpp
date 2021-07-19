//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and Umpire
// project contributors. See the COPYRIGHT file for details.
//
// SPDX-License-Identifier: (MIT)
//////////////////////////////////////////////////////////////////////////////
#ifndef UMPIRE_event_HPP
#define UMPIRE_event_HPP

#include <sstream>
#include <string>
#include <vector>

#include "umpire/Replay.hpp"
#include "umpire/tpl/json/json.hpp"

namespace umpire {
namespace event {

enum class category {operation, statistic};

class event
{
public:
  class builder;

protected:
  std::string m_name{"anon"};
  category m_category{category::statistic};
  std::vector<std::string> m_tags{};
  std::vector<std::pair<std::string, int>> m_int_args{};
  std::vector<std::pair<std::string, std::string>> m_string_args{};
  std::chrono::time_point<std::chrono::system_clock> m_timestamp{};
};

class event::builder
{
public:
  builder() : m_event{}
  {}

  builder& name(const std::string& n) {
    m_event.m_name = n;
    return *this;
  }

  builder& category(event::category c) {
    m_event.m_category = c;
    return *this;
  }

  builder& arg(const std::string& k, int v) {
    m_event.m_int_args.push_back(
      std::make_pair(k, v)
    );
    return *this;
  }

  builder& arg(const std::string& k, const std::string& v) {
    m_event.m_string_args.push_back(
      std::make_pair(k, v)
    );
    return *this;
  }

  builder& arg(const std::string& k, void* p) {
    std::stringstream ss;
    ss << p;  
    std::string pointer{ss.str()}; 
    m_event.m_string_args.push_back(
      std::make_pair(k, pointer)
    );
    return *this;
  }

  builder& tag(const std::string& t) {
    m_event.m_tags.push_back(t);
    return *this;
  }

  void record(const recorder& r)
  {
    m_event.m_timestamp = std::chrono::system_clock::now();

    nlohmann::json je;

    je["name"] = m_event.m_name;
    // TODO: only handles the two event types
    je["category"] = (m_event.m_category == event::category::operation ) ? "operation" : "statistic";
    for (const auto& it : m_event.m_int_args ) {
      std::string name;
      int value;
      std::tie(name, value) = it;
      je["args"][name] = value;
    }

    for (const auto& it : m_event.m_string_args ) {
      std::string name;
      std::string value;
      std::tie(name, value) = it;
      je["args"][name] = value;
    }

    for (const auto& tag : m_event.m_tags) {
      je["tags"] += tag;
    }

    je["timestamp"] = std::to_string(static_cast<long>(std::chrono::time_point_cast<std::chrono::nanoseconds>(m_event.m_timestamp).time_since_epoch().count()));

    std::cout << je << std::endl;

    //umpire::Replay::getReplayLogger()->logMessage(je.dump()); 
  }

private:
  event m_event;
};

}
}
#endif