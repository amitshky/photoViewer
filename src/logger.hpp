#pragma once

#include "raylib.h"

namespace logger {

template<typename... Args>
inline void info(Args&&... args) {
    TraceLog(TraceLogLevel::LOG_INFO, args...);
}

template<typename... Args>
inline void log(Args&&... args) {
    TraceLog(TraceLogLevel::LOG_NONE, args...);
}

template<typename... Args>
inline void error(Args&&... args) {
    TraceLog(TraceLogLevel::LOG_ERROR, args...);
}

template<typename... Args>
inline void warn(Args&&... args) {
    TraceLog(TraceLogLevel::LOG_WARNING, args...);
}

}
