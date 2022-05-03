
#pragma once

#include "cgra_instr.h"
#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {

class ProcessingElement;
class Cgra;

class InstructionMemory {
public:
    InstructionMemory(Cgra *_cgra, ProcessingElement * _pe, uint32_t size);
    ~InstructionMemory() {}
    void setStaticParam(Location& loc, Word param) {
        instructionMemory[loc.inst].setStaticParam(loc, param);
    }
    void loadBitstream(Config& bitstream, std::string key);
    Instruction* getInstruction(InstrMemIdx idx) { return &instructionMemory[idx]; }
    InstrMemIdx size() { return instructionMemory.size(); }

private:
    Cgra *cgra;
    ProcessingElement * pe;
    StrongVec<InstrMemIdx, Instruction> instructionMemory;
};

} // namespace cgra
} // namespace sim
} // namespace platy
