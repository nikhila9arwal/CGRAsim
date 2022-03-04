
#pragma once

#include "engine_cgra_defs.h"
#include "engine_cgra_im.h"
#include "engine_cgra_ts.h"
#include "engine_cgra_event.h"

namespace platy {
namespace sim {
namespace ms {

class ProcessingElement {
public:
    ProcessingElement(
        uint32_t tokenStoreSize, uint32_t instructionMemSize, PeIdx _selfIdx)
        : tokenStore(tokenStoreSize),
          instructionMemory(instructionMemSize),
          selfIdx(_selfIdx) {}

    bool setToken(TokenStore::Token tok);

    // TODO (nikhil): 2 masks what's ready (from tokenStore) and what's missing (from
    // instrMem)

    bool isInstructionReady(
        TokenStore::TokenStoreEntry* tsEntry, InstructionMemory::Instruction* inst);
    void executeInstruction(TokenStore::Tag tag);
    void setStaticParam(Location& loc, Word param) {
        instructionMemory.setStaticParam(loc, param);
    }
    void loadBitstream(Config& bitstream, std::string key) {
        instructionMemory.loadBitstream(bitstream, key);
    }
    void pushFullyImmediateInstructions(CbIdx cbid);

private:
    PeIdx selfIdx;
    TokenStore tokenStore;
    InstructionMemory instructionMemory;
};

}
}
}