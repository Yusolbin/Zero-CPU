#include "zero_cpu/trace/TraceLogger.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace zero_cpu {

TraceLogger::TraceLogger()
    : events_() {
}

void TraceLogger::record(TraceEvent event) {
    events_.push_back(std::move(event));
}

void TraceLogger::clear() {
    events_.clear();
}

bool TraceLogger::empty() const {
    return events_.empty();
}

std::size_t TraceLogger::size() const {
    return events_.size();
}

const std::vector<TraceEvent>& TraceLogger::events() const {
    return events_;
}

const TraceEvent& TraceLogger::last() const {
    if (events_.empty()) {
        throw std::runtime_error("TraceLogger has no events");
    }

    return events_.back();
}

std::string TraceLogger::compactString() const {
    std::ostringstream oss;

    for (const auto& event : events_) {
        oss << event.toCompactString() << "\n";
    }

    return oss.str();
}

std::string TraceLogger::fullString() const {
    std::ostringstream oss;

    for (std::size_t i = 0; i < events_.size(); ++i) {
        oss << "========== Trace Event "
            << i
            << " ==========\n";

        oss << events_[i].toFullString();

        if (i + 1 < events_.size()) {
            oss << "\n";
        }
    }

    return oss.str();
}

} // namespace zero_cpu