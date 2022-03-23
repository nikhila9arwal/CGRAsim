
#pragma once

#include "cgra_defs.h"
#include "cgra_instr_mem.h"
#include "cgra_tok_st.h"
#include "cgra_event.h"
#include "cgra.h"
#include "port.h"
#include "cgra_network.h"


namespace platy {
namespace sim {
namespace cgra {

class ProcessingElement {
public:
    ProcessingElement(
        uint32_t tokenStoreSize, uint32_t instructionMemSize, PeIdx _selfIdx, Cgra* _cgra)
        :   execStage(1, _cgra),
            cgra(_cgra),
            selfIdx(_selfIdx),
            tokenStore(tokenStoreSize),
            instructionMemory(instructionMemSize),
            execLatency(1),
            frontEndLatency(1),
            readyQueueCapacity(1) {}

    ~ProcessingElement() {}
    
    bool acceptToken(TokenStore::Token tok);
    void acknowledgeToken();

    bool isInstructionReady(
        TokenStore::EntryPtr tsEntry, Instruction* inst);
    void executeInstruction(TokenStore::EntryPtr tsEntry);
    void writeback(TokenStore::EntryPtr tsEntry, Word word);

    void setStaticParam(Location& loc, Word param) {
        instructionMemory.setStaticParam(loc, param);
    }
    void loadBitstream(Config& bitstream, std::string key) {
        instructionMemory.loadBitstream(bitstream, key);
    }
    // void pushFullyImmediateInstructions(CbIdx cbid);

private:
    Port execStage;
    Cgra* cgra;
    PeIdx selfIdx;
    TokenStore tokenStore;
    InstructionMemory instructionMemory;

    const Cycles execLatency;
    const Cycles frontEndLatency;

    const size_t readyQueueCapacity;
    std::deque<TokenStore::EntryPtr> readyQueue;

    class ExecutionEvent : public CgraEvent {
        public:
            ExecutionEvent (Cycles _timestamp, ProcessingElement* _pe, TokenStore::EntryPtr &_tsEntry) 
                : CgraEvent(_timestamp), pe(_pe), tsEntry(_tsEntry){}
            void go() { pe->executeInstruction(tsEntry); }
        private:
            ProcessingElement * pe;
            TokenStore::EntryPtr tsEntry;
    
    };

    class WritebackEvent : public CgraEvent {
        public:
        //return here;
            WritebackEvent(Cycles timestamp, ProcessingElement* _pe, TokenStore::EntryPtr _tsEntry, Word _value) 
                : CgraEvent(timestamp), pe(_pe), tsEntry(_tsEntry), value(_value) {}
            void go() { pe->writeback(tsEntry, value); }
        
        private:
            ProcessingElement* pe;
            TokenStore::EntryPtr tsEntry;
            Word value;
    };
};

}
}
}
