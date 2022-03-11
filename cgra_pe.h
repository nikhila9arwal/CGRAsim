
#pragma once

#include "cgra_defs.h"
#include "cgra_im.h"
#include "cgra_ts.h"
#include "cgra_event.h"
#include "cgra.h"

namespace platy {
namespace sim {
namespace cgra {

class ProcessingElement {
public:
    ProcessingElement(
        uint32_t tokenStoreSize, uint32_t instructionMemSize, PeIdx _selfIdx, Cgra* _cgra)
        :   cgra(_cgra),
            selfIdx(_selfIdx),
            tokenStore(tokenStoreSize),
            instructionMemory(instructionMemSize)
           {}

    ~ProcessingElement() {}
    
    bool acceptToken(TokenStore::Token tok);

    bool isInstructionReady(
        std::shared_ptr<TokenStore::TokenStoreEntry> tsEntry, InstructionMemory::Instruction* inst);
    void executeInstruction(std::shared_ptr<TokenStore::TokenStoreEntry> tsEntry);
    void setStaticParam(Location& loc, Word param) {
        instructionMemory.setStaticParam(loc, param);
    }
    void loadBitstream(Config& bitstream, std::string key) {
        instructionMemory.loadBitstream(bitstream, key);
    }
    // void pushFullyImmediateInstructions(CbIdx cbid);

private:
    Cgra* cgra;
    PeIdx selfIdx;
    TokenStore tokenStore;
    InstructionMemory instructionMemory;
};

}
}
}