#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace zero_cpu {

class Memory {
public:
    static constexpr std::size_t kDefaultMemorySize = 1024;

    explicit Memory(std::size_t size = kDefaultMemorySize);

    std::size_t size() const;

    std::int64_t read(std::size_t address) const;
    void write(std::size_t address, std::int64_t value);

    void reset();

    std::vector<std::int64_t> snapshot() const;

    std::string dumpRange(std::size_t start, std::size_t count) const;

private:
    std::vector<std::int64_t> cells_;

    void checkAddress(std::size_t address) const;
};

} // namespace zero_cpu