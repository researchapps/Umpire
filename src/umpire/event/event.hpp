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


namespace umpire {
namespace event {

enum class category {operation, statistic};

class event
{
public:
  class builder;

  event() :
    m_timestamp{std::chrono::system_clock::now()}
  {}

  std::string m_name{"anon"};
  category m_category{category::statistic};
  std::vector<std::string> m_tags{};
  std::vector<std::pair<std::string, int>> m_int_args{};
  std::vector<std::pair<std::string, std::string>> m_string_args{};
  const std::chrono::time_point<std::chrono::system_clock> m_timestamp{};
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

  builder& category(category c) {
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

  template<typename Recorder>
  void record(Recorder&& r)
  {
    r.record(m_event);
  }

private:
  event m_event;
};

}
}
#endif