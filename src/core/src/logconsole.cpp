// Copyright 2016 Chirstopher Torres (Raven), L3nn0x
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// #undef UNICODE
//
#include <sstream>
#include "logconsole.h"

#ifdef _WIN32
  #include <spdlog/sinks/msvc_sink.h>
#else
  #include <spdlog/sinks/syslog_sink.h>
  //#include <spdlog/sinks/systemd_sink.h>
#endif

namespace Core {

spdlog::level::level_enum CLog::level_ = spdlog::level::info;

void CLog::SetLevel(spdlog::level::level_enum _level) {
  level_ = _level;

  std::ostringstream format;
  format << "[%H:%M:%S.%e %z] [%L]";

  if (level_ <= spdlog::level::debug) format << " [thread %t]";
  format << " [%n]" << " %v ";
  spdlog::set_pattern(format.str());
}

std::weak_ptr<spdlog::logger> CLog::GetLogger(
    log_type _type) {
  std::weak_ptr<spdlog::logger> logger;
  try
  {
    switch (_type) {
      case log_type::NETWORK:
        logger = spdlog::get("net");
        break;

      case log_type::DATABASE:
        logger = spdlog::get("db");
        break;

      case log_type::GENERAL:
      default:
        logger = spdlog::get("server");
        break;
    }

    if (logger.expired()) {
      std::ostringstream format;
      format << "[%H:%M:%S.%e.%f %z] [%L]";

      if (level_ <= spdlog::level::debug) format << " [thread %t]";
      format << " [%n]" << " %v ";

      std::string path, name;

      switch (_type) {
        case log_type::NETWORK: {
          path = "logs/network";
          name = "net";
          break;
        }
        case log_type::DATABASE: {
          path = "logs/database";
          name = "db";
          break;
        }
        case log_type::GENERAL:
        default: {
          path = "logs/server";
          name = "server";
          break;
        }
      }
#ifndef DISABLE_ASYNC_LOGGING
      if(spdlog::thread_pool() == nullptr)
        spdlog::init_thread_pool(8192, 1);
#endif
      std::vector<spdlog::sink_ptr> sinks;
      
      auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      sinks.push_back(console_sink);
#ifdef _WIN32
      sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#else
      #ifdef SPDLOG_ENABLE_SYSLOG
        auto syslog_sink = std::make_shared<spdlog::sinks::syslog_sink_mt>(name.c_str());
        syslog_sink->set_level(spdlog::level::warn);
        sinks.push_back(syslog_sink);
      #endif
#endif
#ifndef DISABLE_ASYNC_LOGGING
      auto combined_logger = std::make_shared<spdlog::async_logger>(name.c_str(), sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
#else
      auto combined_logger = std::make_shared<spdlog::logger>(name.c_str(), sinks.begin(), sinks.end());
#endif
      combined_logger->set_level(level_);
      combined_logger->set_pattern(format.str());
      spdlog::register_logger(combined_logger);

      return combined_logger;
    }
  } catch (const spdlog::spdlog_ex& ex) {
    std::cout << "Log failed: " << ex.what() << std::endl;
  }
  return logger;
}
}
