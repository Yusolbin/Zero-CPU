#include "zero_cpu/core/CPUState.hpp"

#include <sstream>
#include <utility>

namespace zero_cpu {

CPUState::CPUState()
    : registers_(),
      memory_(),
      flags_(),
      pc_(0),
      sp_(kDefaultStackBase),
      halted_(false),
      error_(false),
      error_message_() {
}

RegisterFile& CPUState::registers() {
    return registers_;
}

const RegisterFile& CPUState::registers() const {
    return registers_;
}

Memory& CPUState::memory() {
    return memory_;
}

const Memory& CPUState::memory() const {
    return memory_;
}

Flags& CPUState::flags() {
    return flags_;
}

const Flags& CPUState::flags() const {
    return flags_;
}

std::size_t CPUState::pc() const {
    return pc_;
}

void CPUState::setPc(std::size_t value) {
    pc_ = value;
}

void CPUState::advancePc() {
    ++pc_;
}

std::size_t CPUState::sp() const {
    return sp_;
}

void CPUState::setSp(std::size_t value) {
    sp_ = value;
}

bool CPUState::halted() const {
    return halted_;
}

void CPUState::halt() {
    halted_ = true;
}

bool CPUState::hasError() const {
    return error_;
}

const std::string& CPUState::errorMessage() const {
    return error_message_;
}

void CPUState::setError(std::string message) {
    error_ = true;
    halted_ = true;
    error_message_ = std::move(message);
}

void CPUState::reset() {
    registers_.reset();
    memory_.reset();
    flags_.reset();

    pc_ = 0;
    sp_ = kDefaultStackBase;

    halted_ = false;
    error_ = false;
    error_message_.clear();
}

std::string CPUState::summary() const {
    std::ostringstream oss;

    oss << "PC=" << pc_
        << " SP=" << sp_
        << " halted=" << (halted_ ? "true" : "false")
        << " error=" << (error_ ? "true" : "false")
        << "\n";

    oss << "Registers: " << registers_.toString() << "\n";
    oss << "Flags: " << flags_.toString() << "\n";

    if (error_) {
        oss << "Error: " << error_message_ << "\n";
    }

    return oss.str();
}

} // namespace zero_cpu