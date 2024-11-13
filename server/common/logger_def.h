#pragma once

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

static constexpr auto kLogPath = "./logs/";

#define REGISTER_FILE_LOGGER(loggerName, fileName)\
{ \
    const auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(kLogPath + std::string(fileName), 2, 30); \
    fileSink->set_level(spdlog::level::trace); \
    std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};  \
    const auto logger = std::make_shared<spdlog::logger>(loggerName, sinks.begin(), sinks.end()); \
    logger->set_level(spdlog::level::trace); \
    register_logger(logger); \
}


inline void InitLogger() {
    const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::trace);

#ifdef _DEBUG
    REGISTER_FILE_LOGGER("base_logger", "base.log")
#endif
}