#include "zero_cpu/core/CPUState.hpp"

#include <iostream>

int main() {
    zero_cpu::CPUState state;

    std::cout << "=== Zero-CPU Initial State ===\n";
    std::cout << state.summary() << "\n";

    state.registers().set(zero_cpu::RegisterName::R1, 10);
    state.registers().set(zero_cpu::RegisterName::R2, 20);

    const auto result =
        state.registers().get(zero_cpu::RegisterName::R1)
        + state.registers().get(zero_cpu::RegisterName::R2);

    state.registers().set(zero_cpu::RegisterName::R1, result);
    state.flags().updateZeroAndSign(result);
    state.advancePc();

    state.memory().write(100, result);

    std::cout << "=== After Manual ADD-like Operation ===\n";
    std::cout << state.summary();
    std::cout << "Memory: " << state.memory().dumpRange(96, 8) << "\n";

    return 0;
}