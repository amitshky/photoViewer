#pragma once

#include <string>
#include <chrono>

// TODO: create a macro so that you can set it to nothing in Release build
class Timer {
public:
    Timer(const char* title);
    ~Timer();

    void Stop();

private:
    std::string _title;
    std::chrono::time_point<std::chrono::system_clock> _start;
    std::chrono::time_point<std::chrono::system_clock> _end;
};
