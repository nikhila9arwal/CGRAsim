
#pragma once

#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {

class Cgra;

struct Instruction {
    Instruction();
    // Instruction(Config& bitstream, std::string key);
    ~Instruction();
    bool isPredicated;
    bool isLhsImm;
    bool isRhsImm;
    Word lhsImm;
    Word rhsImm;
    Word (*applyFn)(Word lhs, Word rhs);
    
    std::vector<Location> destinations;

    void loadBitstream(Config& bitstream, std::string key, void* functionPtr, Cgra* cgra);
    void decode(std::string type);
    void setStaticParam(Location& loc, Word param);
    bool isFullyImmediate() { return !isPredicated && isLhsImm && isRhsImm; }
};

} // namespace cgra
} // namespace sim
} // namespace platy
