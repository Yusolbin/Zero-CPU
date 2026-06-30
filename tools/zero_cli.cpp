#include "zero_cpu/core/CPUState.hpp"
#include "zero_cpu/isa/Instruction.hpp"
#include "zero_cpu/isa/Opcode.hpp"
#include "zero_cpu/isa/Operand.hpp"

#include <iostream>
#include <vector>

int main() {
    using namespace zero_cpu;

    std::cout << "=== Zero-CPU Initial State ===\n";

    CPUState state;

    std::cout << state.summary() << "\n";

    std::cout << "=== Manual CPU State Test ===\n";

    state.registers().set(RegisterName::R1, 10);
    state.registers().set(RegisterName::R2, 20);

    const auto result =
        state.registers().get(RegisterName::R1)
        + state.registers().get(RegisterName::R2);

    state.registers().set(RegisterName::R1, result);
    state.flags().updateZeroAndSign(result);
    state.advancePc();

    state.memory().write(100, result);

    std::cout << state.summary();
    std::cout << "Memory: " << state.memory().dumpRange(96, 8) << "\n\n";

    std::cout << "=== ISA Instruction Model Test ===\n";

    const Instruction movR1(
        Opcode::MOV,
        Operand::registerOperand(RegisterName::R1),
        Operand::immediate(10)
    );

    const Instruction movR2(
        Opcode::MOV,
        Operand::registerOperand(RegisterName::R2),
        Operand::immediate(20)
    );

    const Instruction add(
        Opcode::ADD,
        Operand::registerOperand(RegisterName::R1),
        Operand::registerOperand(RegisterName::R2)
    );

    const Instruction store(
        Opcode::STORE,
        Operand::memoryAddress(100),
        Operand::registerOperand(RegisterName::R1)
    );

    const Instruction halt(Opcode::HALT);

    std::vector<Instruction> program = {
        movR1,
        movR2,
        add,
        store,
        halt
    };

    for (std::size_t i = 0; i < program.size(); ++i) {
        std::cout << "[" << i << "] "
                  << program[i].toString()
                  << "\n";
    }

    std::cout << "\n";

    std::cout << "=== Opcode Parse Test ===\n";

    const auto parsedAdd = opcodeFromString("add");
    const auto parsedJump = opcodeFromString("JMP");
    const auto parsedInvalid = opcodeFromString("hello");

    std::cout << "add   -> " << opcodeToString(parsedAdd) << "\n";
    std::cout << "JMP   -> " << opcodeToString(parsedJump) << "\n";
    std::cout << "hello -> " << opcodeToString(parsedInvalid) << "\n";

    return 0;
}