
#pragma once

#include "engine_cgra_defs.h"

namespace platy {
namespace sim {
namespace ms {

class InstructionMemory {
public:
    struct Instruction {
        bool isPredicated;
        bool isLhsImm;
        bool isRhsImm;
        Word lhsImm;
        Word rhsImm;
        Word (*applyFn)(Word lhs, Word rhs);
        std::vector<Location> dest;

        void loadBitstream(Config& bitstream, std::string key);
        void decode(std::string type);
        void setStaticParam(Location& loc, Word param);
        bool isFullyImmediate() { return !isPredicated && isLhsImm && isRhsImm; }
    };

    InstructionMemory(uint32_t size);
    void setStaticParam(Location& loc, Word param) {
        instructionMemory[loc.inst].setStaticParam(loc, param);
    }
    void loadBitstream(Config& bitstream, std::string key);
    Instruction* getInstruction(InstrMemIdx idx) { return &instructionMemory[idx]; }
    InstrMemIdx size() { return instructionMemory.size(); }

private:
    StrongVec<InstrMemIdx, Instruction> instructionMemory;
};

}
}
}
