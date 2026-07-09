#include "zero_cpu/core/ALU.hpp"

#include <cstring>
#include <limits>
#include <stdexcept>

namespace zero_cpu {

namespace {

std::int64_t wrapU64ToI64(std::uint64_t value) {
    std::int64_t result = 0;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

bool addSignedOverflow(std::int64_t lhs, std::int64_t rhs, std::int64_t result) {
    return ((rhs > 0) && (lhs > std::numeric_limits<std::int64_t>::max() - rhs)) ||
           ((rhs < 0) && (lhs < std::numeric_limits<std::int64_t>::min() - rhs));
}

bool subSignedOverflow(std::int64_t lhs, std::int64_t rhs, std::int64_t result) {
    (void)result;

    if (rhs > 0) {
        return lhs < std::numeric_limits<std::int64_t>::min() + rhs;
    }

    if (rhs < 0) {
        return lhs > std::numeric_limits<std::int64_t>::max() + rhs;
    }

    return false;
}

bool mulSignedOverflow(std::int64_t lhs, std::int64_t rhs) {
    if (lhs == 0 || rhs == 0) {
        return false;
    }

    if (lhs == -1 && rhs == std::numeric_limits<std::int64_t>::min()) {
        return true;
    }

    if (rhs == -1 && lhs == std::numeric_limits<std::int64_t>::min()) {
        return true;
    }

    const long double result =
        static_cast<long double>(lhs) * static_cast<long double>(rhs);

    return result > static_cast<long double>(std::numeric_limits<std::int64_t>::max()) ||
           result < static_cast<long double>(std::numeric_limits<std::int64_t>::min());
}

} // namespace

ALUResult ALU::makeResult(
    std::int64_t value,
    bool carry,
    bool overflow
) {
    ALUResult result;
    result.value = value;
    result.zero = value == 0;
    result.sign = value < 0;
    result.carry = carry;
    result.overflow = overflow;
    return result;
}

ALUResult ALU::add(std::int64_t lhs, std::int64_t rhs) {
    const std::uint64_t lhs_u = static_cast<std::uint64_t>(lhs);
    const std::uint64_t rhs_u = static_cast<std::uint64_t>(rhs);
    const std::uint64_t result_u = lhs_u + rhs_u;

    const std::int64_t value = wrapU64ToI64(result_u);

    const bool carry = result_u < lhs_u;
    const bool overflow = addSignedOverflow(lhs, rhs, value);

    return makeResult(value, carry, overflow);
}

ALUResult ALU::sub(std::int64_t lhs, std::int64_t rhs) {
    const std::uint64_t lhs_u = static_cast<std::uint64_t>(lhs);
    const std::uint64_t rhs_u = static_cast<std::uint64_t>(rhs);
    const std::uint64_t result_u = lhs_u - rhs_u;

    const std::int64_t value = wrapU64ToI64(result_u);

    const bool carry = lhs_u < rhs_u;
    const bool overflow = subSignedOverflow(lhs, rhs, value);

    return makeResult(value, carry, overflow);
}

ALUResult ALU::mul(std::int64_t lhs, std::int64_t rhs) {
    const std::uint64_t lhs_u = static_cast<std::uint64_t>(lhs);
    const std::uint64_t rhs_u = static_cast<std::uint64_t>(rhs);
    const std::uint64_t result_u = lhs_u * rhs_u;

    const std::int64_t value = wrapU64ToI64(result_u);

    const bool overflow = mulSignedOverflow(lhs, rhs);
    const bool carry = overflow;

    return makeResult(value, carry, overflow);
}

ALUResult ALU::div(std::int64_t lhs, std::int64_t rhs) {
    if (rhs == 0) {
        throw std::runtime_error("ALU division by zero");
    }

    if (lhs == std::numeric_limits<std::int64_t>::min() && rhs == -1) {
        throw std::runtime_error("ALU signed division overflow");
    }

    const std::int64_t value = lhs / rhs;

    return makeResult(value, false, false);
}

ALUResult ALU::bitAnd(std::int64_t lhs, std::int64_t rhs) {
    return makeResult(lhs & rhs, false, false);
}

ALUResult ALU::bitOr(std::int64_t lhs, std::int64_t rhs) {
    return makeResult(lhs | rhs, false, false);
}

ALUResult ALU::bitXor(std::int64_t lhs, std::int64_t rhs) {
    return makeResult(lhs ^ rhs, false, false);
}

ALUResult ALU::bitNot(std::int64_t value) {
    return makeResult(~value, false, false);
}

ALUResult ALU::compare(std::int64_t lhs, std::int64_t rhs) {
    return sub(lhs, rhs);
}

ALUResult ALU::test(std::int64_t lhs, std::int64_t rhs) {
    return bitAnd(lhs, rhs);
}

} // namespace zero_cpu