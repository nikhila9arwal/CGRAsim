
#pragma once

#include "cgra_defs.h"
#include "cgra_instr_mem.h"
#include "cgra_tok_st.h"
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
            instructionMemory(instructionMemSize),
            readyToExecute(true)
           {}

    ~ProcessingElement() {}
    
    bool acceptToken(TokenStore::Token tok);
    void acknowledgeToken();

    bool isInstructionReady(
        TokenStore::EntryPtr tsEntry, Instruction* inst);
    void executeInstruction(TokenStore::EntryPtr tsEntry);
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

    const Cycles execLatency;

    Port execStage;
    const size_t readyQueueCapacity;
    std::deque<TokenStore::EntryPtr> readyQueue;

    class ExecutionEvent : public CgraEvent {
      public:
        void go() override { pe->executeInstruction(...); }
    };

    class WritebackEvent : public CgraEvent {
      public:
        void go() override { pe->writeback(...); }
    };
};

}
}
}
