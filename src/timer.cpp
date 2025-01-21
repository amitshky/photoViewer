#include "timer.hpp"

#include "logger.hpp"

Timer::Timer(const std::string& title)
    : _title{ title },
      _start{ std::chrono::system_clock::now() },
      _end{} {
}

Timer::~Timer() {
    Stop();
}

void Timer::Stop() {
    _end = std::chrono::system_clock::now();

    const std::chrono::duration<double> duration =
        std::chrono::duration_cast<std::chrono::microseconds>(_end - _start);
    const auto ms = duration.count() / 1e-3;
    const auto us = duration.count() / 1e-6;

    logger::info("\"%s\" duration = %.4fs (%.4fms | %.2fus)",
        _title.c_str(), duration.count(), ms, us);
}
