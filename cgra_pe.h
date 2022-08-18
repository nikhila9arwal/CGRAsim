
#pragma once

#include "cgra_defs.h"
#include "cgra_instr_mem.h"
#include "cgra_tok_st.h"
#include "cgra_event.h"
#include "port.h"
#include "cgra_network_port.h"


namespace platy {
namespace sim {
namespace cgra {

class Cgra;

class ProcessingElement : public NetworkPort {
public:
    ProcessingElement(
        uint32_t tokenStoreSize, uint32_t instructionMemSize, PeIdx _selfIdx, Cgra* _cgra);
    ~ProcessingElement() {}
    bool acceptToken(TokenStore::Token tok);
    int getId() override;
    void acknowledgeToken() override;
    bool isInstructionReady(
        TokenStore::EntryPtr tsEntry, Instruction* inst);
    void executeInstruction();
    void writeback(TokenStore::EntryPtr tsEntry, Word word);
    void setStaticParam(Location& loc, Word param);
    void loadBitstream(Config& bitstream, std::string key, void* functionPtr);
    bool tagMatch(TokenStore::Token tok);


private:
    Cgra* cgra;
    PeIdx selfIdx;
    Port tagMatchStage;
    Port execStage;
    TokenStore tokenStore;
    InstructionMemory instructionMemory;

    const Cycles execLatency;
    const Cycles frontEndLatency;

    const size_t readyQueueCapacity;
    std::deque<TokenStore::EntryPtr> readyQueue;

    class TagMatchEvent : public CgraEvent {
    public:
        TagMatchEvent (ProcessingElement* _pe, TokenStore::Token _tok);
        void go() override;
        void printInfo() override;
    private:
        ProcessingElement* pe;
        TokenStore::Token tok;
    };

    class ExecutionEvent : public CgraEvent {
    public:
        ExecutionEvent (ProcessingElement* _pe);
        void go() override;
        void printInfo() override;

    private:
        ProcessingElement* pe;
    };
    class WritebackEvent : public CgraEvent {
    public:
        WritebackEvent(ProcessingElement* _pe, TokenStore::EntryPtr _tsEntry, Word _value);
        void go() override;
        void printInfo() override;

    private:
        ProcessingElement* pe;
        TokenStore::EntryPtr tsEntry;
        Word value;
    };
};

}  // namespace cgra
}  // namespace sim
}  // namespace platy
