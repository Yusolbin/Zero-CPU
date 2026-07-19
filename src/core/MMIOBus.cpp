#include "zero_cpu/core/MMIOBus.hpp"

#include <limits>
#include <stdexcept>

namespace zero_cpu {

namespace {

std::size_t checkedEnd(std::size_t base, std::size_t size) {
    if (size == 0) {
        throw std::runtime_error("MMIO mapping size must be greater than zero");
    }

    if (base > std::numeric_limits<std::size_t>::max() - size) {
        throw std::runtime_error("MMIO mapping address range overflow");
    }

    return base + size;
}

bool rangesOverlap(
    std::size_t aBase,
    std::size_t aSize,
    std::size_t bBase,
    std::size_t bSize
) {
    const std::size_t aEnd = checkedEnd(aBase, aSize);
    const std::size_t bEnd = checkedEnd(bBase, bSize);

    return aBase < bEnd && bBase < aEnd;
}

} // namespace

void MMIOBus::clear() {
    mappings_.clear();
}

void MMIOBus::mapDevice(
    std::size_t base,
    std::size_t size,
    std::shared_ptr<MMIODevice> device
) {
    validateMapping(base, size, device);

    MMIOMapping mapping;
    mapping.base = base;
    mapping.size = size;
    mapping.device = std::move(device);

    mappings_.push_back(std::move(mapping));
}

bool MMIOBus::hasDeviceAt(std::size_t address) const {
    for (const MMIOMapping& mapping : mappings_) {
        if (mapping.contains(address)) {
            return true;
        }
    }

    return false;
}

std::int64_t MMIOBus::read(std::size_t address) {
    MMIOMapping& mapping = findMapping(address);
    return mapping.device->read(address - mapping.base);
}

void MMIOBus::write(std::size_t address, std::int64_t value) {
    MMIOMapping& mapping = findMapping(address);
    mapping.device->write(address - mapping.base, value);
}

const std::vector<MMIOMapping>& MMIOBus::mappings() const {
    return mappings_;
}

MMIOMapping& MMIOBus::findMapping(std::size_t address) {
    for (MMIOMapping& mapping : mappings_) {
        if (mapping.contains(address)) {
            return mapping;
        }
    }

    throw std::runtime_error("No MMIO device mapped at address");
}

const MMIOMapping& MMIOBus::findMapping(std::size_t address) const {
    for (const MMIOMapping& mapping : mappings_) {
        if (mapping.contains(address)) {
            return mapping;
        }
    }

    throw std::runtime_error("No MMIO device mapped at address");
}

void MMIOBus::validateMapping(
    std::size_t base,
    std::size_t size,
    const std::shared_ptr<MMIODevice>& device
) const {
    checkedEnd(base, size);

    if (!device) {
        throw std::runtime_error("MMIO device must not be null");
    }

    for (const MMIOMapping& existing : mappings_) {
        if (rangesOverlap(base, size, existing.base, existing.size)) {
            throw std::runtime_error(
                "MMIO mapping overlaps with existing device: " +
                existing.device->name()
            );
        }
    }
}

} // namespace zero_cpu