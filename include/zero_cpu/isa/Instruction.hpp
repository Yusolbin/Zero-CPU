#pragma once

#include "zero_cpu/isa/Opcode.hpp"
#include "zero_cpu/isa/Operand.hpp"

#include <string>

namespace zero_cpu {

class Instruction {
public:
    Instruction();
    explicit Instruction(Opcode opcode);
    Instruction(Opcode opcode, Operand dst);
    Instruction(Opcode opcode, Operand dst, Operand src);

    Opcode opcode() const;
    const Operand& dst() const;
    const Operand& src() const;

    bool hasDestination() const;
    bool hasSource() const;

    std::string toString() const;

private:
    Opcode opcode_;
    Operand dst_;
    Operand src_;
};

} // namespace zero_cpu