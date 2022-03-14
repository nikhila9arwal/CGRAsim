

#include "cgra_instr_mem.h"

namespace platy {
namespace sim {
namespace cgra {

InstructionMemory::InstructionMemory(uint32_t size)
    : instructionMemory{(InstrMemIdx)size} {
}

void InstructionMemory::loadBitstream(Config& bitstream, std::string key) {
    for (InstrMemIdx i = 0_instid;; i++) {
        std::string configKey = key + qformat(".inst_{}", i);
        if (bitstream.exists(configKey)) {
            qassert(i < instructionMemory.size());
            instructionMemory[i].loadBitstream(bitstream, configKey);
        } else {
            break;
        }
    }
}

}
}
}
