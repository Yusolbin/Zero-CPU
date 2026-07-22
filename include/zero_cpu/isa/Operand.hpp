#pragma once

#include "zero_cpu/core/RegisterFile.hpp"

#include <cstdint>
#include <cstddef>
#include <string>

namespace zero_cpu {

enum class OperandType {
    Register,
    Immediate,
    MemoryAddress,
    RegisterIndirectAddress,
    Label,
    None
};

class Operand {
public:
    Operand();

    static Operand none();
    static Operand registerOperand(RegisterName reg);
    static Operand immediate(std::int64_t value);
    static Operand memoryAddress(std::size_t address);
    static Operand registerIndirectAddress(RegisterName reg);
    static Operand label(std::string name);

    OperandType type() const;

    bool isRegister() const;
    bool isImmediate() const;
    bool isMemoryAddress() const;
    bool isRegisterIndirectAddress() const;
    bool isLabel() const;
    bool isNone() const;

    RegisterName asRegister() const;
    std::int64_t asImmediate() const;
    std::size_t asMemoryAddress() const;
    RegisterName asRegisterIndirectBase() const;
    const std::string& asLabel() const;

    std::string toString() const;

private:
    OperandType type_;
    RegisterName reg_;
    std::int64_t immediate_;
    std::size_t address_;
    std::string label_;

    void ensureType(OperandType expected) const;
};

std::string operandTypeToString(OperandType type);

} // namespace zero_cpu