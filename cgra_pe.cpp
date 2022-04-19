
#include "cgra_pe.h"

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
    // qassert(timeAcquire == cgra->now());
    qassert(!readyQueue.empty());

    auto tsEntry = readyQueue.front();
    readyQueue.pop_front();


    auto instruction = instructionMemory.getInstruction(tsEntry->tag.instIdx);
    Word lhs = instruction->isLhsImm ? instruction->lhsImm : tsEntry->lhs;
    Word rhs = instruction->isRhsImm ? instruction->rhsImm : tsEntry->rhs;
    Word output = instruction->applyFn(lhs, rhs);

    std::cout<<"PE, Inst, Timestamp, Cbid = "<<selfIdx<<", "<<tsEntry->tag.instIdx<<", "<<cgra->now()<<", "<<tsEntry->tag.cbid;
    std::cout<<"\t Output = "<< output << "\n";

    Cycles timestamp = cgra->now();
    auto* wbEvent = new WritebackEvent{this, tsEntry, output};
    cgra->pushEvent(wbEvent, timestamp);
}

void ProcessingElement::writeback(TokenStore::EntryPtr tsEntry, Word word) {
    auto inst = instructionMemory.getInstruction(tsEntry->tag.instIdx);
    cgra->getNetwork()->sendToken(this, inst->destinations, word, tsEntry->tag.cbid);
}


}
}
}
