#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include <string>

namespace zero_cpu {

enum class RegisterName : std::size_t {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    Count
};

class RegisterFile {
public:
    static constexpr std::size_t kRegisterCount =
        static_cast<std::size_t>(RegisterName::Count);

    RegisterFile();

    std::int64_t get(RegisterName reg) const;
    void set(RegisterName reg, std::int64_t value);

    void reset();

    std::array<std::int64_t, kRegisterCount> snapshot() const;

    std::string toString() const;

    static bool isValidIndex(std::size_t index);
    static std::string registerNameToString(RegisterName reg);

private:
    std::array<std::int64_t, kRegisterCount> registers_;

    static std::size_t toIndex(RegisterName reg);
};

} // namespace zero_cpu