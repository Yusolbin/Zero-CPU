#pragma once

#include <cstdint>
#include <string>

namespace zero_cpu {

class Flags {
public:
    Flags();

    bool zero() const;
    bool sign() const;
    bool overflow() const;
    bool carry() const;

    void setZero(bool value);
    void setSign(bool value);
    void setOverflow(bool value);
    void setCarry(bool value);

    void reset();

    void updateZeroAndSign(std::int64_t result);

    std::string toString() const;

private:
    bool zero_flag_;
    bool sign_flag_;
    bool overflow_flag_;
    bool carry_flag_;
};

} // namespace zero_cpu