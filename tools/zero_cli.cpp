#include "zero_cpu/core/CPU.hpp"
#include "zero_cpu/core/RegisterFile.hpp"
#include "zero_cpu/isa/Instruction.hpp"
#include "zero_cpu/isa/Opcode.hpp"
#include "zero_cpu/isa/Operand.hpp"

#include <iostream>
#include <vector>

namespace {

void printProgram(const std::vector<zero_cpu::Instruction>& program) {
    std::cout << "=== Zero-CPU Program ===\n";

    for (std::size_t i = 0; i < program.size(); ++i) {
        std::cout << "[" << i << "] "
                  << program[i].toString()
                  << "\n";
    }

    std::cout << "\n";
}

void runStepByStep(zero_cpu::CPU& cpu) {
    std::cout << "=== Step Execution ===\n";

    std::size_t stepCount = 0;

    while (!cpu.state().halted()) {
        const std::size_t pcBefore = cpu.state().pc();

        if (pcBefore >= cpu.program().size()) {
            std::cout << "PC out of program range.\n";
            break;
        }

        const zero_cpu::Instruction& instruction = cpu.program()[pcBefore];

        std::cout << "Step " << stepCount
                  << " | PC=" << pcBefore
                  << " | " << instruction.toString()
                  << "\n";

        cpu.step();

        std::cout << cpu.state().summary();
        std::cout << "Memory[96..103]: "
                  << cpu.state().memory().dumpRange(96, 8)
                  << "\n\n";

        ++stepCount;
    }
}

} // namespace

int main() {
    using namespace zero_cpu;

    std::vector<Instruction> program = {
        Instruction(
            Opcode::MOV,
            Operand::registerOperand(RegisterName::R1),
            Operand::immediate(123)
        ),

        Instruction(
            Opcode::STORE,
            Operand::memoryAddress(100),
            Operand::registerOperand(RegisterName::R1)
        ),

        Instruction(
            Opcode::LOAD,
            Operand::registerOperand(RegisterName::R2),
            Operand::memoryAddress(100)
        ),

        Instruction(
            Opcode::MUL,
            Operand::registerOperand(RegisterName::R2),
            Operand::immediate(2)
        ),

        Instruction(
            Opcode::DIV,
            Operand::registerOperand(RegisterName::R2),
            Operand::immediate(3)
        ),

        Instruction(
            Opcode::SUB,
            Operand::registerOperand(RegisterName::R2),
            Operand::immediate(82)
        ),

        Instruction(Opcode::HALT)
    };

    CPU cpu;
    cpu.loadProgram(program);

    printProgram(cpu.program());

    std::cout << "=== Initial CPU State ===\n";
    std::cout << cpu.state().summary();
    std::cout << "Memory[96..103]: "
              << cpu.state().memory().dumpRange(96, 8)
              << "\n\n";

    runStepByStep(cpu);

    std::cout << "=== Final CPU State ===\n";
    std::cout << cpu.state().summary();
    std::cout << "Memory[96..103]: "
              << cpu.state().memory().dumpRange(96, 8)
              << "\n\n";

    if (cpu.state().hasError()) {
        std::cout << "Execution failed: "
                  << cpu.state().errorMessage()
                  << "\n";

        return 1;
    }

    std::cout << "Execution finished successfully.\n";

    return 0;
}