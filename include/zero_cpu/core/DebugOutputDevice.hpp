#pragma once

#include "zero_cpu/core/MMIODevice.hpp"

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace zero_cpu {

class DebugOutputDevice final : public MMIODevice {
public:
    std::string name() const override;

    std::int64_t read(std::size_t offset) override;
    void write(std::size_t offset, std::int64_t value) override;

    const std::vector<std::int64_t>& writes() const;
    std::string outputText() const;

    void clear();

private:
    std::vector<std::int64_t> writes_;
    std::ostringstream output_;
};

} // namespace zero_cpu