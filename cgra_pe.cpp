
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
    
    // do a ready check
    auto instruction = instructionMemory.getInstruction(tok.tag.instIdx);
    if (isInstructionReady(tokenStoreEntry, instruction)) {
        //TODO: set and send would be different events. Send would be handled by network.
        //Network delay would only be known by the network
        if(!instruction->isPredicated || tokenStoreEntry->predicate) {
            if (!execStage.empty()) {
                Cycles execTime = execStage.acquire() + frontEndLatency;
                CgraEvent * execEvent = new ExecutionEvent{execTime, this, tokenStoreEntry};
                cgra->pushEvent(execEvent);
            } else {
                readyQueue.push_back(tokenStoreEntry);
            }
        }
        tokenStore.erase(tok.tag);
    }
    return true;
}

void ProcessingElement::acknowledgeToken() {
    qassert(execStage.empty());

    execStage.release(0_cycles /* exec latency modeled in execute below */);

    if (!readyQueue.empty()) {
        auto tokenStoreEntry = readyQueue.front();
        readyQueue.pop_front();

        // auto inst = instructionMemory.getInstruction(tokenStoreEntry->tag.instIdx);

        // TODO (nzb): Front-end delay already paid above???
        Cycles execTime = execStage.acquire() /* + cgra->frontEndDelay*/;
        auto* execEvent = new ExecutionEvent{execTime, this, tokenStoreEntry};
        cgra->pushEvent(execEvent);
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

void ProcessingElement::executeInstruction(TokenStore::EntryPtr tsEntry) {
    // auto tsEntry = tokenStore.getTokenStoreEntry(tag);
    auto instruction = instructionMemory.getInstruction(tsEntry->tag.instIdx);
    Word lhs = instruction->isLhsImm ? instruction->lhsImm : tsEntry->lhs;

    Word rhs = instruction->isRhsImm ? instruction->rhsImm : tsEntry->rhs;
    Word output = instruction->applyFn(lhs, rhs);

    std::cout<<"PE, Inst, Timestamp = "<<selfIdx<<", "<<tsEntry->tag.instIdx<<", "<<cgra->now();
    std::cout<<"\t Output = "<< output << "\n";

    auto* wbEvent = new WritebackEvent{ cgra->now() + execLatency, this, tsEntry, output };
    cgra->pushEvent(wbEvent);
}

void ProcessingElement::writeback(TokenStore::EntryPtr tsEntry, Word word) {
    auto inst = instructionMemory.getInstruction(tsEntry->tag.instIdx);
    
    cgra->getNetwork()->sendToken(this, inst->destinations, word, tsEntry->tag.cbid);
}

// void ProcessingElement::pushFullyImmediateInstructions(CbIdx cbid) {
//     for (InstrMemIdx i = 0_instid; i < instructionMemory.size(); i++) {
//         if (instructionMemory.getInstruction(i)->isFullyImmediate()) {
//             CgraEvent* event = new ExecuteCgraEvent(currentTime, selfIdx, i, cbid);
//             cgra->pushEvent(event);
//         }
//     }
// }

}
}
}
