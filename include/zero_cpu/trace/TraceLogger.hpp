#pragma once

#include "zero_cpu/trace/TraceEvent.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace zero_cpu {

class TraceLogger {
public:
    TraceLogger();

    void record(TraceEvent event);
    void clear();

    bool empty() const;
    std::size_t size() const;

    const std::vector<TraceEvent>& events() const;
    const TraceEvent& last() const;

    std::string compactString() const;
    std::string fullString() const;

private:
    std::vector<TraceEvent> events_;
};

} // namespace zero_cpu