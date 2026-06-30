#include "zero_cpu/isa/Operand.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace zero_cpu {

Operand::Operand()
    : type_(OperandType::None),
      reg_(RegisterName::R0),
      immediate_(0),
      address_(0),
      label_() {
}

Operand Operand::none() {
    return Operand();
}

Operand Operand::registerOperand(RegisterName reg) {
    Operand operand;
    operand.type_ = OperandType::Register;
    operand.reg_ = reg;
    return operand;
}

Operand Operand::immediate(std::int64_t value) {
    Operand operand;
    operand.type_ = OperandType::Immediate;
    operand.immediate_ = value;
    return operand;
}

Operand Operand::memoryAddress(std::size_t address) {
    Operand operand;
    operand.type_ = OperandType::MemoryAddress;
    operand.address_ = address;
    return operand;
}

Operand Operand::label(std::string name) {
    Operand operand;
    operand.type_ = OperandType::Label;
    operand.label_ = std::move(name);
    return operand;
}

OperandType Operand::type() const {
    return type_;
}

bool Operand::isRegister() const {
    return type_ == OperandType::Register;
}

bool Operand::isImmediate() const {
    return type_ == OperandType::Immediate;
}

bool Operand::isMemoryAddress() const {
    return type_ == OperandType::MemoryAddress;
}

bool Operand::isLabel() const {
    return type_ == OperandType::Label;
}

bool Operand::isNone() const {
    return type_ == OperandType::None;
}

RegisterName Operand::asRegister() const {
    ensureType(OperandType::Register);
    return reg_;
}

std::int64_t Operand::asImmediate() const {
    ensureType(OperandType::Immediate);
    return immediate_;
}

std::size_t Operand::asMemoryAddress() const {
    ensureType(OperandType::MemoryAddress);
    return address_;
}

const std::string& Operand::asLabel() const {
    ensureType(OperandType::Label);
    return label_;
}

std::string Operand::toString() const {
    std::ostringstream oss;

    switch (type_) {
    case OperandType::Register:
        oss << RegisterFile::registerNameToString(reg_);
        break;

    case OperandType::Immediate:
        oss << immediate_;
        break;

    case OperandType::MemoryAddress:
        oss << "[" << address_ << "]";
        break;

    case OperandType::Label:
        oss << label_;
        break;

    case OperandType::None:
        oss << "<none>";
        break;
    }

    return oss.str();
}

void Operand::ensureType(OperandType expected) const {
    if (type_ != expected) {
        throw std::logic_error(
            "Invalid operand access: expected "
            + operandTypeToString(expected)
            + ", got "
            + operandTypeToString(type_)
        );
    }
}

std::string operandTypeToString(OperandType type) {
    switch (type) {
    case OperandType::Register:
        return "Register";
    case OperandType::Immediate:
        return "Immediate";
    case OperandType::MemoryAddress:
        return "MemoryAddress";
    case OperandType::Label:
        return "Label";
    case OperandType::None:
        return "None";
    }

    return "Unknown";
}

} // namespace zero_cpu