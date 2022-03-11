
#pragma once

#include "cgra_defs.h"
#include "cgra_instr.h"

namespace platy {
namespace sim {
namespace cgra {

class InstructionMemory {
public:
    InstructionMemory(uint32_t size);
    ~InstructionMemory() {}
    void setStaticParam(Location& loc, Word param) {
        instructionMemory[loc.inst].setStaticParam(loc, param);
    }
    void loadBitstream(Config& bitstream, std::string key);
    Instruction* getInstruction(InstrMemIdx idx) { return &instructionMemory[idx]; }
    InstrMemIdx size() { return instructionMemory.size(); }

private:
    StrongVec<InstrMemIdx, Instruction> instructionMemory;
};

} // namespace cgra
} // namespace sim
} // namespace platy
