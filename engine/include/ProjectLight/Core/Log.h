#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <utility>

namespace ProjectLight
{
    class Log
    {
    public:
        Log();
        ~Log();

        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;
        Log(Log&&) = delete;
        Log& operator=(Log&&) = delete;

        template <typename... Arguments>
        static void Trace(spdlog::format_string_t<Arguments...> format, Arguments&&... arguments) {
            s_Logger->trace(format, std::forward<Arguments>(arguments)...);
        }

        template <typename... Arguments>
        static void Debug(spdlog::format_string_t<Arguments...> format, Arguments&&... arguments) {
            s_Logger->debug(format, std::forward<Arguments>(arguments)...);
        }

        template <typename... Arguments>
        static void Info(spdlog::format_string_t<Arguments...> format, Arguments&&... arguments) {
            s_Logger->info(format, std::forward<Arguments>(arguments)...);
        }

        template <typename... Arguments>
        static void Warn(spdlog::format_string_t<Arguments...> format, Arguments&&... arguments) {
            s_Logger->warn(format, std::forward<Arguments>(arguments)...);
        }

        template <typename... Arguments>
        static void Error(spdlog::format_string_t<Arguments...> format, Arguments&&... arguments) {
            s_Logger->error(format, std::forward<Arguments>(arguments)...);
        }

        template <typename... Arguments>
        static void Critical(spdlog::format_string_t<Arguments...> format,
                             Arguments&&... arguments) {
            s_Logger->critical(format, std::forward<Arguments>(arguments)...);
        }

    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
}
