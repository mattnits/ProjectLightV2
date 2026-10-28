#include "ProjectLight/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace ProjectLight
{
    std::shared_ptr<spdlog::logger> Log::s_Logger;

    Log::Log() {
        s_Logger = spdlog::stdout_color_mt("ProjectLight");
        s_Logger->set_pattern("%^[%T] [%n] [%l] %v%$");
#ifndef NDEBUG
        s_Logger->set_level(spdlog::level::trace);
#else
        s_Logger->set_level(spdlog::level::info);
#endif
        s_Logger->flush_on(spdlog::level::err);

        Info("Logging initialized");
    }

    Log::~Log() {
        if (s_Logger == nullptr) {
            return;
        }

        Info("Logging shutting down");
        s_Logger->flush();
        spdlog::drop(s_Logger->name());
        s_Logger.reset();
    }
}
