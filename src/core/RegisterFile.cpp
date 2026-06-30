#include "zero_cpu/core/RegisterFile.hpp"

#include <sstream>
#include <stdexcept>

namespace zero_cpu {

RegisterFile::RegisterFile() {
    reset();
}

std::int64_t RegisterFile::get(RegisterName reg) const {
    return registers_.at(toIndex(reg));
}

void RegisterFile::set(RegisterName reg, std::int64_t value) {
    registers_.at(toIndex(reg)) = value;
}

void RegisterFile::reset() {
    registers_.fill(0);
}

std::array<std::int64_t, RegisterFile::kRegisterCount> RegisterFile::snapshot() const {
    return registers_;
}

std::string RegisterFile::toString() const {
    std::ostringstream oss;

    for (std::size_t i = 0; i < kRegisterCount; ++i) {
        auto reg = static_cast<RegisterName>(i);
        oss << registerNameToString(reg) << "=" << registers_[i];

        if (i + 1 < kRegisterCount) {
            oss << " ";
        }
    }

    return oss.str();
}

bool RegisterFile::isValidIndex(std::size_t index) {
    return index < kRegisterCount;
}

std::string RegisterFile::registerNameToString(RegisterName reg) {
    switch (reg) {
    case RegisterName::R0:
        return "R0";
    case RegisterName::R1:
        return "R1";
    case RegisterName::R2:
        return "R2";
    case RegisterName::R3:
        return "R3";
    case RegisterName::R4:
        return "R4";
    case RegisterName::R5:
        return "R5";
    case RegisterName::R6:
        return "R6";
    case RegisterName::R7:
        return "R7";
    case RegisterName::Count:
        break;
    }

    throw std::out_of_range("Invalid register name");
}

std::size_t RegisterFile::toIndex(RegisterName reg) {
    auto index = static_cast<std::size_t>(reg);

    if (!isValidIndex(index)) {
        throw std::out_of_range("Register index out of range");
    }

    return index;
}

} // namespace zero_cpu