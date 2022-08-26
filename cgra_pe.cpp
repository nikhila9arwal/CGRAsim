
#include "cgra_pe.h"
#include "cgra.h"
#include "cgra_network.h"

#define DEBUG_CGRA_PE
#ifdef DEBUG_CGRA_PE
#define DBG(...) qlog(__VA_ARGS__)
#else
#define DBG(...)
#endif

/*
 token comes in
 frontend acquire
 frontend event 

 frontend event:
    release frontend
    push to rq
    create exec event and acquire exec if exec free

exec event:
    pop from rq
    execute
    create writeback

writeback event:
    try to send


if send
    call acknowledge
        release exec
        create new exec events if rq not empty
    



*/

namespace platy {
namespace sim {
namespace cgra {

ProcessingElement::ProcessingElement(
        uint32_t tokenStoreSize, uint32_t instructionMemSize, PeIdx _selfIdx, Cgra* _cgra)
        :   cgra(_cgra),
            selfIdx(_selfIdx),
            tagMatchStage(1),
            execStage(1),
            tokenStore(tokenStoreSize),
            instructionMemory(this, instructionMemSize),
            execLatency(1),
            frontEndLatency(1),
            readyQueueCapacity(100) {}

int ProcessingElement::getId() {return int(selfIdx);}

void ProcessingElement::setStaticParam(Location& loc, Word param) {
    instructionMemory.setStaticParam(loc, param);
}

void ProcessingElement::loadBitstream(Config& bitstream, std::string key, void* functionPtr) {
    instructionMemory.loadBitstream(bitstream, key, functionPtr, cgra);
}

bool ProcessingElement::tagMatch(TokenStore::Token tok){
    // qassert(!tagMatchStage.isAvailable());

    auto tokenStoreEntry = tokenStore.acceptToken(tok);
    if (tokenStoreEntry == nullptr) {
        return false;
    }
    auto instruction = instructionMemory.getInstruction(tok.tag.instIdx);
    if (isInstructionReady(tokenStoreEntry, instruction)) {
        if(!instruction->isPredicated || tokenStoreEntry->predicate) {
            readyQueue.push_back(tokenStoreEntry);
            if (execStage.isAvailable()) {
                Cycles execTime = execStage.acquire() + execLatency;
                CgraEvent* execEvent = new ExecutionEvent{this};
                cgra->pushEvent(execEvent, execTime);
            }
        }
        tokenStore.erase(tok.tag);
    }
    
    tagMatchStage.release(0_cycles);
    return true;
}


bool ProcessingElement::acceptToken(TokenStore::Token tok) {
    
    if (readyQueueCapacity == readyQueue.size() || !tagMatchStage.isAvailable()) {
        return false;
    }

    Cycles tagMatchTime = tagMatchStage.acquire()+frontEndLatency;
    CgraEvent* tagMatchEvent = new TagMatchEvent{this, tok};
    cgra->pushEvent(tagMatchEvent, tagMatchTime);
    return true;
}

void ProcessingElement::acknowledgeToken() {
    qassert(!execStage.isAvailable());

    execStage.release(0_cycles /* exec latency modeled in execute below */);

    if (!readyQueue.empty()) {
        Cycles execTime = execStage.acquire() + execLatency;
        auto* execEvent = new ExecutionEvent{this};
        cgra->pushEvent(execEvent, execTime);
    }
}

bool ProcessingElement::isInstructionReady(
    TokenStore::EntryPtr tsEntry, Instruction* inst) {
    if (inst->isPredicated && !tsEntry->predicateValid) {
        return false;
    }
    if (!inst->isLhsImm && !tsEntry->lhsValid) {
        return false;
    }
    if (!inst->isRhsImm && !tsEntry->rhsValid) {
        return false;
    }
    return true;
}

void ProcessingElement::executeInstruction() {
    // auto tsEntry = tokenStore.getTokenStoreEntry(tag);
    // Cycles timeAcquire = execStage.acquire();
    // qassert(timeAcquire == events::now());
    qassert(!readyQueue.empty());


    TokenStore::EntryPtr tsEntry = readyQueue.front();
    TokenStore::Tag tag = tsEntry->tag;
    readyQueue.pop_front();
    Instruction * instruction = instructionMemory.getInstruction(tag.instIdx);
    Word lhs = instruction->isLhsImm ? instruction->lhsImm : tsEntry->lhs;
    Word rhs = instruction->isRhsImm ? instruction->rhsImm : tsEntry->rhs;

        

    auto fn = [instruction, lhs, rhs, tag, this](){
        Word output = instruction->applyFn(lhs, rhs, tag.cbid, cgra); 
        DBG("PE {} Inst {} Timestamp {} Cbid {} Output {} lhs {} rhs {} ", selfIdx, tag.instIdx, 
            events::now(), tag.cbid, output, lhs, rhs);
        writeback(tag, output); 
    }; 
    //TODO (nikhil): make opcode an enum.
    if(instruction->opcode ==  "LOAD" || instruction->opcode ==  "STORE"){
        spawn_event(fn);
    } else {
        fn();
    }

    // Cycles timestamp = events::now();
    // auto* wbEvent = new WritebackEvent{this, tsEntry, output};
    // cgra->pushEvent(wbEvent, timestamp);
}

void ProcessingElement::writeback(TokenStore::Tag tag, Word word) {
    auto inst = instructionMemory.getInstruction(tag.instIdx);
    cgra->getNetwork()->sendToken(this, inst->destinations, word, tag.cbid);
}

ProcessingElement::TagMatchEvent::TagMatchEvent (ProcessingElement* _pe, TokenStore::Token _tok)
    : CgraEvent(), pe(_pe), tok(_tok) {}

void ProcessingElement::TagMatchEvent::go(){
            pe->tagMatch(tok); 
        }
void ProcessingElement::TagMatchEvent::printInfo(){
    DBG("TagMatchEvent Source = {} \n", int(pe->getId()));
}

ProcessingElement::ExecutionEvent::ExecutionEvent (ProcessingElement* _pe)
    : CgraEvent(), pe(_pe) {}

void ProcessingElement::ExecutionEvent::go()  { 
    pe->executeInstruction(); 
}
void ProcessingElement::ExecutionEvent::printInfo()  {
    DBG("ExecutionEvent Source = {} \n", int(pe->getId()));
}

ProcessingElement::WritebackEvent::WritebackEvent(ProcessingElement* _pe, TokenStore::Tag _tag, Word _value)
    : CgraEvent(), pe(_pe), tag(_tag), value(_value) {}
void ProcessingElement::WritebackEvent::go() { 
    pe->writeback(tag, value); 
}
void ProcessingElement::WritebackEvent::printInfo() {
    DBG("WritebackEvent Source = {} \n", int(pe->getId()));
}

}
}
}
