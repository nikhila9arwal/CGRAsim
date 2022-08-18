
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
    InstructionMemory(ProcessingElement * _pe, uint32_t size);
    ~InstructionMemory() {}
    void setStaticParam(Location& loc, Word param);
    void loadBitstream(Config& bitstream, std::string key, void* functionPtr, Cgra* cgra);
    Instruction* getInstruction(InstrMemIdx idx);
    InstrMemIdx size();

private:
    ProcessingElement * pe;
    StrongVec<InstrMemIdx, Instruction> instructionMemory;
};

} // namespace cgra
} // namespace sim
} // namespace platy
