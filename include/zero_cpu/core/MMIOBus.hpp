#pragma once

#include "zero_cpu/core/MMIODevice.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace zero_cpu {

struct MMIOMapping {
    std::size_t base = 0;
    std::size_t size = 0;
    std::shared_ptr<MMIODevice> device;

    std::size_t endExclusive() const {
        return base + size;
    }

    bool contains(std::size_t address) const {
        return address >= base && address < endExclusive();
    }
};

class MMIOBus {
public:
    void clear();

    void mapDevice(
        std::size_t base,
        std::size_t size,
        std::shared_ptr<MMIODevice> device
    );

    bool hasDeviceAt(std::size_t address) const;

    std::int64_t read(std::size_t address);
    void write(std::size_t address, std::int64_t value);

    const std::vector<MMIOMapping>& mappings() const;

private:
    std::vector<MMIOMapping> mappings_;

    MMIOMapping& findMapping(std::size_t address);
    const MMIOMapping& findMapping(std::size_t address) const;

    void validateMapping(
        std::size_t base,
        std::size_t size,
        const std::shared_ptr<MMIODevice>& device
    ) const;
};

} // namespace zero_cpu