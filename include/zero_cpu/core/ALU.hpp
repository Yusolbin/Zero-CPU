#pragma once

#include <cstdint>

namespace zero_cpu {

struct ALUResult {
    std::int64_t value = 0;

    bool zero = false;
    bool sign = false;
    bool carry = false;
    bool overflow = false;
};

class ALU {
public:
    static ALUResult add(std::int64_t lhs, std::int64_t rhs);
    static ALUResult sub(std::int64_t lhs, std::int64_t rhs);
    static ALUResult mul(std::int64_t lhs, std::int64_t rhs);
    static ALUResult div(std::int64_t lhs, std::int64_t rhs);

    static ALUResult bitAnd(std::int64_t lhs, std::int64_t rhs);
    static ALUResult bitOr(std::int64_t lhs, std::int64_t rhs);
    static ALUResult bitXor(std::int64_t lhs, std::int64_t rhs);
    static ALUResult bitNot(std::int64_t value);

    static ALUResult compare(std::int64_t lhs, std::int64_t rhs);
    static ALUResult test(std::int64_t lhs, std::int64_t rhs);

private:
    static ALUResult makeResult(
        std::int64_t value,
        bool carry,
        bool overflow
    );
};

} // namespace zero_cpu