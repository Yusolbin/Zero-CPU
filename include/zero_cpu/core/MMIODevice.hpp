#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace zero_cpu {

class MMIODevice {
public:
    virtual ~MMIODevice() = default;

    virtual std::string name() const = 0;

    virtual std::int64_t read(std::size_t offset) = 0;
    virtual void write(std::size_t offset, std::int64_t value) = 0;
};

} // namespace zero_cpu