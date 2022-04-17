
#include "cgra_pe.h"


namespace platy {
namespace sim {
namespace cgra {

bool ProcessingElement::acceptToken(TokenStore::Token tok) {
    
    if (readyQueueCapacity == readyQueue.size()) {
        return false;
    }
    auto tokenStoreEntry = tokenStore.acceptToken(tok);
    if (tokenStoreEntry == nullptr) {
        return false;
    }


    auto instruction = instructionMemory.getInstruction(tok.tag.instIdx);
    if (isInstructionReady(tokenStoreEntry, instruction)) {
        if(!instruction->isPredicated || tokenStoreEntry->predicate) {
            Cycles frontendReady = cgra->now() + frontEndLatency;
            readyQueue.push_back(std::pair<TokenStore::EntryPtr, Cycles>(tokenStoreEntry,frontendReady));
            if (execStage.isAvailable()) {
                Cycles execReady = execStage.acquire();
                Cycles execTime = std::max(frontendReady, execReady);
                CgraEvent* execEvent = new ExecutionEvent{this};
                cgra->pushEvent(execEvent, execTime);
            }
        }
        tokenStore.erase(tok.tag);
    }
    return true;
}

void ProcessingElement::acknowledgeToken() {
    qassert(!execStage.isAvailable());

    execStage.release(0_cycles /* exec latency modeled in execute below */);

    if (!readyQueue.empty()) {
        Cycles frontendReady = readyQueue.front().second;
        Cycles execReady = execStage.acquire();
        //has front end latency been paid already V
        Cycles execTime = std::max(frontendReady, execReady);
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

    auto tsEntry = readyQueue.front().first;
    readyQueue.pop_front();


    auto instruction = instructionMemory.getInstruction(tsEntry->tag.instIdx);
    Word lhs = instruction->isLhsImm ? instruction->lhsImm : tsEntry->lhs;
    Word rhs = instruction->isRhsImm ? instruction->rhsImm : tsEntry->rhs;
    Word output = instruction->applyFn(lhs, rhs);

    std::cout<<"PE, Inst, Timestamp, Cbid = "<<selfIdx<<", "<<tsEntry->tag.instIdx<<", "<<cgra->now()<<", "<<tsEntry->tag.cbid;
    std::cout<<"\t Output = "<< output << "\n";

    Cycles timestamp = cgra->now() + execLatency;
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
