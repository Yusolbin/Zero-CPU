#include "zero_cpu/binary/BinaryLoader.hpp"

#include "zero_cpu/binary/BinaryFormat.hpp"

#include <stdexcept>

namespace zero_cpu {
namespace binary {

LoadedBinaryImage BinaryLoader::loadIntoMemory(
    const BinaryProgram& program,
    Memory& memory,
    std::size_t codeBase
) const {
    validateProgram(program);

    memory.writeBytes(codeBase, program.code);

    LoadedBinaryImage image;
    image.code_base = codeBase;
    image.entry_point =
        codeBase + static_cast<std::size_t>(program.header.entry_point);
    image.code_size = program.code.size();

    return image;
}

void BinaryLoader::validateProgram(const BinaryProgram& program) const {
    if (program.header.endianness != BinaryEndianness::Little) {
        throw std::runtime_error(
            "BinaryLoader currently supports only little-endian binaries"
        );
    }

    if (program.code.empty()) {
        throw std::runtime_error("Binary code section is empty");
    }

    if (
        static_cast<std::size_t>(program.header.code_size)
        != program.code.size()
    ) {
        throw std::runtime_error(
            "Binary header code_size does not match actual code size"
        );
    }

    if (program.code.size() % kInstructionSize != 0) {
        throw std::runtime_error(
            "Binary code size must be a multiple of instruction size"
        );
    }

    if (program.header.entry_point >= program.header.code_size) {
        throw std::runtime_error(
            "Binary entry point is outside the code section"
        );
    }

    if (program.header.entry_point % kInstructionAlignment != 0) {
        throw std::runtime_error(
            "Binary entry point is not instruction-aligned"
        );
    }
}

} // namespace binary
} // namespace zero_cpu