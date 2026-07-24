#include "zero_cpu/trace/TraceEvent.hpp"

#include "zero_cpu/core/RegisterFile.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <utility>

namespace zero_cpu {

TraceEvent::TraceEvent(
    CPUState before,
    Instruction instruction,
    CPUState after,
    std::string error_message
)
    : before_(std::move(before)),
      instruction_(std::move(instruction)),
      after_(std::move(after)),
      changed_registers_(),
      changed_flags_(),
      changed_memory_(),
      stage_(),
      action_(),
      datapath_nodes_(),
      error_message_(std::move(error_message)) {
    analyzeChanges();
    analyzeVisualMetadata();
}

const CPUState& TraceEvent::before() const {
    return before_;
}

const CPUState& TraceEvent::after() const {
    return after_;
}

const Instruction& TraceEvent::instruction() const {
    return instruction_;
}

std::size_t TraceEvent::pcBefore() const {
    return before_.pc();
}

std::size_t TraceEvent::pcAfter() const {
    return after_.pc();
}

const std::vector<RegisterChange>& TraceEvent::changedRegisters() const {
    return changed_registers_;
}

const std::vector<FlagChange>& TraceEvent::changedFlags() const {
    return changed_flags_;
}

const std::vector<MemoryChange>& TraceEvent::changedMemory() const {
    return changed_memory_;
}

const std::string& TraceEvent::stage() const {
    return stage_;
}

const std::string& TraceEvent::action() const {
    return action_;
}

const std::vector<std::string>& TraceEvent::datapathNodes() const {
    return datapath_nodes_;
}

std::string TraceEvent::datapathString() const {
    return joinDatapathNodes(datapath_nodes_);
}

bool TraceEvent::hasError() const {
    return !error_message_.empty();
}

const std::string& TraceEvent::errorMessage() const {
    return error_message_;
}

std::string TraceEvent::toCompactString() const {
    std::ostringstream oss;

    oss << "PC=" << formatPc(pcBefore())
        << " | " << instruction_.toString();

    if (!stage_.empty()) {
        oss << " | stage=" << stage_;
    }

    if (!action_.empty()) {
        oss << " | action=" << action_;
    }

    if (!datapath_nodes_.empty()) {
        oss << " | path=" << datapathString();
    }

    for (const auto& change : changed_registers_) {
        oss << " | "
            << change.name
            << ":"
            << change.before
            << "->"
            << change.after;
    }

    for (const auto& change : changed_flags_) {
        oss << " | "
            << change.name
            << ":"
            << (change.before ? 1 : 0)
            << "->"
            << (change.after ? 1 : 0);
    }

    if (before_.sp() != after_.sp()) {
        oss << " | SP:"
            << before_.sp()
            << "->"
            << after_.sp();
    }

    if (before_.halted() != after_.halted()) {
        oss << " | halted:"
            << boolToString(before_.halted())
            << "->"
            << boolToString(after_.halted());
    }

    if (hasError()) {
        oss << " | ERROR: " << error_message_;
    }

    oss << " | NextPC=" << formatPc(pcAfter());

    return oss.str();
}

std::string TraceEvent::toFullString() const {
    std::ostringstream oss;

    oss << "[PC=" << formatPc(pcBefore()) << "] "
        << instruction_.toString()
        << "\n\n";

    oss << "Visual Metadata:\n";
    oss << "  Stage: "
        << (stage_.empty() ? "Unknown" : stage_)
        << "\n";
    oss << "  Action: "
        << (action_.empty() ? "Unknown" : action_)
        << "\n";
    oss << "  Datapath: "
        << (datapath_nodes_.empty() ? "None" : datapathString())
        << "\n\n";

    oss << "Before:\n";
    oss << before_.summary();

    oss << "\nAfter:\n";
    oss << after_.summary();

    oss << "\nChanged Registers:\n";
    if (changed_registers_.empty()) {
        oss << "  None\n";
    } else {
        for (const auto& change : changed_registers_) {
            oss << "  "
                << change.name
                << ": "
                << change.before
                << " -> "
                << change.after
                << "\n";
        }
    }

    oss << "\nChanged Flags:\n";
    if (changed_flags_.empty()) {
        oss << "  None\n";
    } else {
        for (const auto& change : changed_flags_) {
            oss << "  "
                << change.name
                << ": "
                << (change.before ? 1 : 0)
                << " -> "
                << (change.after ? 1 : 0)
                << "\n";
        }
    }

    oss << "\nChanged Memory:\n";
    if (changed_memory_.empty()) {
        oss << "  None\n";
    } else {
        for (const auto& change : changed_memory_) {
            oss << "  Memory["
                << change.address
                << "]: "
                << change.before
                << " -> "
                << change.after
                << "\n";
        }
    }

    if (before_.sp() != after_.sp()) {
        oss << "\nChanged Stack Pointer:\n";
        oss << "  SP: "
            << before_.sp()
            << " -> "
            << after_.sp()
            << "\n";
    }

    oss << "\nNextPC:\n";
    oss << "  " << formatPc(pcAfter()) << "\n";

    oss << "\nError:\n";
    if (hasError()) {
        oss << "  " << error_message_ << "\n";
    } else {
        oss << "  None\n";
    }

    return oss.str();
}

void TraceEvent::analyzeChanges() {
    analyzeRegisterChanges();
    analyzeFlagChanges();
    analyzeMemoryChanges();
}

void TraceEvent::analyzeRegisterChanges() {
    const auto before_registers = before_.registers().snapshot();
    const auto after_registers = after_.registers().snapshot();

    for (std::size_t i = 0; i < RegisterFile::kRegisterCount; ++i) {
        const auto before_value = before_registers[i];
        const auto after_value = after_registers[i];

        if (before_value != after_value) {
            auto reg = static_cast<RegisterName>(i);

            changed_registers_.push_back(
                RegisterChange{
                    RegisterFile::registerNameToString(reg),
                    before_value,
                    after_value
                }
            );
        }
    }
}

void TraceEvent::analyzeFlagChanges() {
    const auto& before_flags = before_.flags();
    const auto& after_flags = after_.flags();

    if (before_flags.zero() != after_flags.zero()) {
        changed_flags_.push_back(
            FlagChange{"ZF", before_flags.zero(), after_flags.zero()}
        );
    }

    if (before_flags.sign() != after_flags.sign()) {
        changed_flags_.push_back(
            FlagChange{"SF", before_flags.sign(), after_flags.sign()}
        );
    }

    if (before_flags.overflow() != after_flags.overflow()) {
        changed_flags_.push_back(
            FlagChange{"OF", before_flags.overflow(), after_flags.overflow()}
        );
    }

    if (before_flags.carry() != after_flags.carry()) {
        changed_flags_.push_back(
            FlagChange{"CF", before_flags.carry(), after_flags.carry()}
        );
    }
}

void TraceEvent::analyzeMemoryChanges() {
    const auto before_memory = before_.memory().snapshot();
    const auto after_memory = after_.memory().snapshot();

    const std::size_t count =
        before_memory.size() < after_memory.size()
            ? before_memory.size()
            : after_memory.size();

    for (std::size_t address = 0; address < count; ++address) {
        const auto before_value = before_memory[address];
        const auto after_value = after_memory[address];

        if (before_value != after_value) {
            changed_memory_.push_back(
                MemoryChange{
                    address,
                    before_value,
                    after_value
                }
            );
        }
    }
}

void TraceEvent::analyzeVisualMetadata() {
    stage_ = "EXECUTE";
    action_ = "UNKNOWN";
    datapath_nodes_.clear();

    addDatapathNode("PC");
    addDatapathNode("InstructionMemory");
    addDatapathNode("Decoder");

    switch (instruction_.opcode()) {
    case Opcode::NOP:
        action_ = "NOP";
        addDatapathNode("ControlUnit");
        break;

    case Opcode::HALT:
        action_ = "HALT";
        addDatapathNode("ControlUnit");
        break;

    case Opcode::MOV:
        action_ = "REGISTER_TRANSFER";
        addDatapathNode("RegisterFile");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::LOAD:
        action_ = "MEMORY_READ";
        addDatapathNode("AddressUnit");
        addDatapathNode("Memory/MMIO");
        addDatapathNode("RegisterFile");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::STORE:
        action_ = "MEMORY_WRITE";
        addDatapathNode("AddressUnit");
        addDatapathNode("RegisterFile");
        addDatapathNode("Memory/MMIO");
        addDatapathNode("Flags");
        break;

    case Opcode::ADD:
        action_ = "ALU_ADD";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::SUB:
        action_ = "ALU_SUB";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::MUL:
        action_ = "ALU_MUL";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::DIV:
        action_ = "ALU_DIV";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::CMP:
        action_ = "ALU_COMPARE";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        break;

    case Opcode::TEST:
        action_ = "ALU_TEST";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        break;

    case Opcode::JMP:
        action_ = "JUMP";
        addDatapathNode("ControlUnit");
        addDatapathNode("PC");
        break;

    case Opcode::JE:
    case Opcode::JNE:
    case Opcode::JG:
    case Opcode::JL:
        action_ = "CONDITIONAL_BRANCH";
        addDatapathNode("Flags");
        addDatapathNode("ControlUnit");
        addDatapathNode("PC");
        break;

    case Opcode::PUSH:
        action_ = "STACK_PUSH";
        addDatapathNode("RegisterFile");
        addDatapathNode("Stack");
        addDatapathNode("SP");
        break;

    case Opcode::POP:
        action_ = "STACK_POP";
        addDatapathNode("Stack");
        addDatapathNode("SP");
        addDatapathNode("RegisterFile");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::CALL:
        action_ = "CALL";
        addDatapathNode("ControlUnit");
        addDatapathNode("Stack");
        addDatapathNode("PC");
        break;

    case Opcode::RET:
        action_ = "RETURN";
        addDatapathNode("Stack");
        addDatapathNode("PC");
        break;

    case Opcode::INT:
        action_ = "SOFTWARE_INTERRUPT";
        addDatapathNode("InterruptController");
        addDatapathNode("Stack");
        addDatapathNode("Flags");
        addDatapathNode("PC");
        break;

    case Opcode::IRET:
        action_ = "INTERRUPT_RETURN";
        addDatapathNode("Stack");
        addDatapathNode("Flags");
        addDatapathNode("PC");
        break;

    case Opcode::EI:
        action_ = "INTERRUPT_ENABLE";
        addDatapathNode("InterruptController");
        addDatapathNode("ControlUnit");
        break;

    case Opcode::DI:
        action_ = "INTERRUPT_DISABLE";
        addDatapathNode("InterruptController");
        addDatapathNode("ControlUnit");
        break;

    case Opcode::AND:
        action_ = "ALU_AND";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::OR:
        action_ = "ALU_OR";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::XOR:
        action_ = "ALU_XOR";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::NOT:
        action_ = "ALU_NOT";
        addDatapathNode("RegisterFile");
        addDatapathNode("ALU");
        addDatapathNode("Flags");
        addDatapathNode("Writeback");
        break;

    case Opcode::Invalid:
    default:
        action_ = "INVALID";
        addDatapathNode("ControlUnit");
        break;
    }

    if (!changed_registers_.empty()) {
        addDatapathNode("RegisterFile");
    }

    if (!changed_flags_.empty()) {
        addDatapathNode("Flags");
    }

    if (!changed_memory_.empty()) {
        addDatapathNode("Memory");
    }

    if (before_.sp() != after_.sp()) {
        addDatapathNode("Stack");
        addDatapathNode("SP");
    }

    if (before_.pc() != after_.pc()) {
        addDatapathNode("PC");
    }
}

void TraceEvent::addDatapathNode(std::string node) {
    if (
        std::find(
            datapath_nodes_.begin(),
            datapath_nodes_.end(),
            node
        ) == datapath_nodes_.end()
    ) {
        datapath_nodes_.push_back(std::move(node));
    }
}

std::string TraceEvent::boolToString(bool value) {
    return value ? "true" : "false";
}

std::string TraceEvent::formatPc(std::size_t pc) {
    std::ostringstream oss;

    oss << std::setw(4)
        << std::setfill('0')
        << pc;

    return oss.str();
}

std::string TraceEvent::joinDatapathNodes(
    const std::vector<std::string>& nodes
) {
    std::ostringstream oss;

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (i > 0) {
            oss << " -> ";
        }

        oss << nodes[i];
    }

    return oss.str();
}

} // namespace zero_cpu
