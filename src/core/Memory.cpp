#include "zero_cpu/core/Memory.hpp"

#include <sstream>
#include <stdexcept>

namespace zero_cpu {

Memory::Memory(std::size_t size)
    : cells_(size, 0) {
}

std::size_t Memory::size() const {
    return cells_.size();
}

std::int64_t Memory::read(std::size_t address) const {
    checkAddress(address);
    return cells_[address];
}

void Memory::write(std::size_t address, std::int64_t value) {
    checkAddress(address);
    cells_[address] = value;
}

void Memory::reset() {
    for (auto& cell : cells_) {
        cell = 0;
    }
}

std::vector<std::int64_t> Memory::snapshot() const {
    return cells_;
}

std::string Memory::dumpRange(std::size_t start, std::size_t count) const {
    if (start >= cells_.size()) {
        throw std::out_of_range("Memory dump start address out of range");
    }

    std::ostringstream oss;

    const std::size_t end = (start + count > cells_.size())
        ? cells_.size()
        : start + count;

    for (std::size_t address = start; address < end; ++address) {
        oss << "[" << address << "]=" << cells_[address];

        if (address + 1 < end) {
            oss << " ";
        }
    }

    return oss.str();
}

void Memory::checkAddress(std::size_t address) const {
    if (address >= cells_.size()) {
        throw std::out_of_range("Memory address out of range");
    }
}

} // namespace zero_cpu