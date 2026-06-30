#include "zero_cpu/core/Flags.hpp"

#include <sstream>

namespace zero_cpu {

Flags::Flags()
    : zero_flag_(false),
      sign_flag_(false),
      overflow_flag_(false),
      carry_flag_(false) {
}

bool Flags::zero() const {
    return zero_flag_;
}

bool Flags::sign() const {
    return sign_flag_;
}

bool Flags::overflow() const {
    return overflow_flag_;
}

bool Flags::carry() const {
    return carry_flag_;
}

void Flags::setZero(bool value) {
    zero_flag_ = value;
}

void Flags::setSign(bool value) {
    sign_flag_ = value;
}

void Flags::setOverflow(bool value) {
    overflow_flag_ = value;
}

void Flags::setCarry(bool value) {
    carry_flag_ = value;
}

void Flags::reset() {
    zero_flag_ = false;
    sign_flag_ = false;
    overflow_flag_ = false;
    carry_flag_ = false;
}

void Flags::updateZeroAndSign(std::int64_t result) {
    zero_flag_ = (result == 0);
    sign_flag_ = (result < 0);
}

std::string Flags::toString() const {
    std::ostringstream oss;

    oss << "ZF=" << (zero_flag_ ? 1 : 0)
        << " SF=" << (sign_flag_ ? 1 : 0)
        << " OF=" << (overflow_flag_ ? 1 : 0)
        << " CF=" << (carry_flag_ ? 1 : 0);

    return oss.str();
}

} // namespace zero_cpu