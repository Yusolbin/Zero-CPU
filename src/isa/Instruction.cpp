#include "zero_cpu/isa/Instruction.hpp"

#include <sstream>

namespace zero_cpu {

Instruction::Instruction()
    : opcode_(Opcode::Invalid),
      dst_(Operand::none()),
      src_(Operand::none()) {
}

Instruction::Instruction(Opcode opcode)
    : opcode_(opcode),
      dst_(Operand::none()),
      src_(Operand::none()) {
}

Instruction::Instruction(Opcode opcode, Operand dst)
    : opcode_(opcode),
      dst_(std::move(dst)),
      src_(Operand::none()) {
}

Instruction::Instruction(Opcode opcode, Operand dst, Operand src)
    : opcode_(opcode),
      dst_(std::move(dst)),
      src_(std::move(src)) {
}

Opcode Instruction::opcode() const {
    return opcode_;
}

const Operand& Instruction::dst() const {
    return dst_;
}

const Operand& Instruction::src() const {
    return src_;
}

bool Instruction::hasDestination() const {
    return !dst_.isNone();
}

bool Instruction::hasSource() const {
    return !src_.isNone();
}

std::string Instruction::toString() const {
    std::ostringstream oss;

    oss << opcodeToString(opcode_);

    if (hasDestination()) {
        oss << " " << dst_.toString();
    }

    if (hasSource()) {
        oss << ", " << src_.toString();
    }

    return oss.str();
}

} // namespace zero_cpu