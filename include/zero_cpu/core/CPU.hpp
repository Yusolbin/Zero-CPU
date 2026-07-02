#pragma once

#include "zero_cpu/core/CPUState.hpp"
#include "zero_cpu/isa/Instruction.hpp"
#include "zero_cpu/isa/Operand.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace zero_cpu {

class CPU {
public:
    CPU();

    void loadProgram(std::vector<Instruction> program);
    void reset();

    CPUState& state();
    const CPUState& state() const;

    const std::vector<Instruction>& program() const;

    bool step();
    void run(std::size_t max_steps = 100000);

private:
    CPUState state_;
    std::vector<Instruction> program_;

    void execute(const Instruction& instruction);

    void executeNop(const Instruction& instruction);
    void executeHalt(const Instruction& instruction);

    void executeMov(const Instruction& instruction);
    void executeLoad(const Instruction& instruction);
    void executeStore(const Instruction& instruction);

    void executeAdd(const Instruction& instruction);
    void executeSub(const Instruction& instruction);
    void executeMul(const Instruction& instruction);
    void executeDiv(const Instruction& instruction);

    std::int64_t readOperandValue(const Operand& operand) const;
    void writeOperandValue(const Operand& operand, std::int64_t value);

    void requireNoOperand(const Instruction& instruction) const;
    void requireDestination(const Instruction& instruction) const;
    void requireSource(const Instruction& instruction) const;

    void requireRegisterDestination(const Instruction& instruction) const;
    void requireMemoryDestination(const Instruction& instruction) const;
    void requireMemorySource(const Instruction& instruction) const;
};

} // namespace zero_cpu