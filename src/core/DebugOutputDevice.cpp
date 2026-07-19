#include "zero_cpu/core/DebugOutputDevice.hpp"

#include <stdexcept>

namespace zero_cpu {

std::string DebugOutputDevice::name() const {
    return "DebugOutputDevice";
}

std::int64_t DebugOutputDevice::read(std::size_t offset) {
    if (offset == 0) {
        if (writes_.empty()) {
            return 0;
        }

        return writes_.back();
    }

    if (offset == 8) {
        return static_cast<std::int64_t>(writes_.size());
    }

    throw std::runtime_error(
        "DebugOutputDevice read offset must be 0 or 8"
    );
}

void DebugOutputDevice::write(std::size_t offset, std::int64_t value) {
    if (offset != 0) {
        throw std::runtime_error(
            "DebugOutputDevice write offset must be 0"
        );
    }

    writes_.push_back(value);
    output_ << value << "\n";
}

const std::vector<std::int64_t>& DebugOutputDevice::writes() const {
    return writes_;
}

std::string DebugOutputDevice::outputText() const {
    return output_.str();
}

void DebugOutputDevice::clear() {
    writes_.clear();
    output_.str("");
    output_.clear();
}

} // namespace zero_cpu