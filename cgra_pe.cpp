
#include "cgra_pe.h"


namespace platy {
namespace sim {
namespace cgra {

bool ProcessingElement::acceptToken(TokenStore::Token tok) {
    auto tokenStoreEntry = tokenStore.acceptToken(tok);
    if (tokenStoreEntry == nullptr) {
        return false;
    }
    // do a ready check
    auto instruction = instructionMemory.getInstruction(tok.tag.instIdx);
    if (isInstructionReady(tokenStoreEntry, instruction)) {
        //TODO: set and send would be different events. Send would be handled by network.
        //Network delay would only be known by the network
        if(!instruction->isPredicated || tokenStoreEntry->predicate){
            Cycles newEventTime = (cgra->currentTime + cgra->networkDelay + cgra->setTokenDelay);
            CgraEvent* newEvent = new ExecuteCgraEvent(newEventTime, selfIdx, tokenStoreEntry);
            cgra->pushEvent(newEvent);
        }
        tokenStore.erase(tok.tag);
    }
    return true;
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


    std::cout<<"PE, Inst, Timestamp = "<<selfIdx<<", "<<tsEntry->tag.instIdx<<", "<<cgra->currentTime;
    std::cout<<"\t Output = "<< output << "\n";

    //TODO (nikhil): List of destinations should be sent out along with the output
    for (auto loc : instruction->dest) {
        TokenStore::Token tok{loc.pos, output, loc.inst, tsEntry->tag.cbid};
        CgraEvent*  newEvent =  new SendTokenCgraEvent(cgra->currentTime + cgra->executionDelay, loc.pe, tok);
        cgra->pushEvent(newEvent);
    }
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
