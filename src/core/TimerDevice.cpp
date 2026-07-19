#include "zero_cpu/core/TimerDevice.hpp"

#include <limits>
#include <stdexcept>

namespace zero_cpu {

TimerDevice::TimerDevice(
    std::shared_ptr<InterruptController> controller,
    std::uint8_t vector,
    std::uint64_t interval,
    std::int64_t payload
)
    : controller_(std::move(controller)),
      vector_(vector),
      interval_(interval),
      payload_(payload) {
    validateController();

    if (interval_ == 0) {
        throw std::runtime_error("TimerDevice interval must be greater than zero");
    }
}

std::string TimerDevice::name() const {
    return "TimerDevice";
}

std::int64_t TimerDevice::read(std::size_t offset) {
    switch (offset) {
    case kTickCountOffset:
        return static_cast<std::int64_t>(tick_count_);
    case kIntervalOffset:
        return static_cast<std::int64_t>(interval_);
    case kEnabledOffset:
        return enabled_ ? 1 : 0;
    case kVectorOffset:
        return static_cast<std::int64_t>(vector_);
    case kPayloadOffset:
        return payload_;
    case kInterruptCountOffset:
        return static_cast<std::int64_t>(interrupt_count_);
    default:
        throw std::runtime_error("Unsupported TimerDevice read offset");
    }
}

void TimerDevice::write(std::size_t offset, std::int64_t value) {
    switch (offset) {
    case kTickCountOffset:
        if (value < 0) {
            throw std::runtime_error("TimerDevice tick count must not be negative");
        }
        tick_count_ = static_cast<std::uint64_t>(value);
        return;

    case kIntervalOffset:
        if (value <= 0) {
            throw std::runtime_error("TimerDevice interval must be greater than zero");
        }
        interval_ = static_cast<std::uint64_t>(value);
        return;

    case kEnabledOffset:
        enabled_ = value != 0;
        return;

    case kVectorOffset:
        if (value < 0 || value > static_cast<std::int64_t>(std::numeric_limits<std::uint8_t>::max())) {
            throw std::runtime_error("TimerDevice vector must be in range 0..255");
        }
        vector_ = static_cast<std::uint8_t>(value);
        return;

    case kPayloadOffset:
        payload_ = value;
        return;

    case kInterruptCountOffset:
        if (value != 0) {
            throw std::runtime_error("TimerDevice interrupt count register only accepts 0 for reset");
        }
        interrupt_count_ = 0;
        return;

    default:
        throw std::runtime_error("Unsupported TimerDevice write offset");
    }
}

void TimerDevice::tick() {
    if (tick_count_ == std::numeric_limits<std::uint64_t>::max()) {
        throw std::runtime_error("TimerDevice tick counter overflow");
    }

    ++tick_count_;
    maybeRequestInterrupt();
}

void TimerDevice::tick(std::uint64_t count) {
    for (std::uint64_t i = 0; i < count; ++i) {
        tick();
    }
}

void TimerDevice::reset() {
    tick_count_ = 0;
    interrupt_count_ = 0;
    enabled_ = true;
}

bool TimerDevice::enabled() const {
    return enabled_;
}

void TimerDevice::setEnabled(bool enabled) {
    enabled_ = enabled;
}

std::uint64_t TimerDevice::tickCount() const {
    return tick_count_;
}

std::uint64_t TimerDevice::interval() const {
    return interval_;
}

void TimerDevice::setInterval(std::uint64_t interval) {
    if (interval == 0) {
        throw std::runtime_error("TimerDevice interval must be greater than zero");
    }

    interval_ = interval;
}

std::uint8_t TimerDevice::vector() const {
    return vector_;
}

void TimerDevice::setVector(std::uint8_t vector) {
    vector_ = vector;
}

std::int64_t TimerDevice::payload() const {
    return payload_;
}

void TimerDevice::setPayload(std::int64_t payload) {
    payload_ = payload;
}

std::uint64_t TimerDevice::interruptCount() const {
    return interrupt_count_;
}

void TimerDevice::validateController() const {
    if (!controller_) {
        throw std::runtime_error("TimerDevice requires a valid InterruptController");
    }
}

void TimerDevice::maybeRequestInterrupt() {
    if (!enabled_) {
        return;
    }

    if (interval_ == 0) {
        throw std::runtime_error("TimerDevice interval must be greater than zero");
    }

    if (tick_count_ % interval_ != 0) {
        return;
    }

    controller_->request(vector_, payload_, "timer");
    ++interrupt_count_;
}

} // namespace zero_cpu