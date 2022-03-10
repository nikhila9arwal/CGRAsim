
#include "engine_cgra_pe.h"


namespace platy {
namespace sim {
namespace ms {

bool ProcessingElement::setToken(TokenStore::Token tok) {
    auto tokenStoreEntry = tokenStore.setToken(tok);
    if (tokenStoreEntry == nullptr) {
        return false;
    }
    // do a ready check
    auto instruction = instructionMemory.getInstruction(tok.tag.instIdx);
    if (isInstructionReady(tokenStoreEntry, instruction)) {
        uint32_t newEventTime = (currentTime + networkDelay + setTokenDelay);
        CgraEvent* event = new ExecuteCgraEvent(newEventTime, selfIdx, tok.tag.instIdx, tok.tag.cbid);
        cgra->pushToCgraQueue(event);
    }
    return true;
}

bool ProcessingElement::isInstructionReady(
    TokenStore::TokenStoreEntry* tsEntry, InstructionMemory::Instruction* inst) {
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

void ProcessingElement::executeInstruction(TokenStore::Tag tag) {
    auto tsEntry = tokenStore.getTokenStoreEntry(tag);
    auto instruction = instructionMemory.getInstruction(tag.instIdx);
    Word lhs = instruction->isLhsImm ? instruction->lhsImm : tsEntry->lhs;
    Word rhs = instruction->isRhsImm ? instruction->rhsImm : tsEntry->rhs;
    Word output = instruction->applyFn(lhs, rhs);
    std::cout<<"PE, Inst, Timestamp = "<<selfIdx<<", "<<tag.instIdx<<", "<<currentTime<<"\n";
    for (auto loc : instruction->dest) {
        TokenStore::Token tok(loc.pos, output, loc.inst, tag.cbid);
        CgraEvent*  event =  new SendTokenCgraEvent(currentTime + executionDelay, loc.pe, tok);
        cgra->pushToCgraQueue(event);

    }
}

void ProcessingElement::pushFullyImmediateInstructions(CbIdx cbid) {
    for (InstrMemIdx i = 0_instid; i < instructionMemory.size(); i++) {
        if (instructionMemory.getInstruction(i)->isFullyImmediate()) {
            CgraEvent* event = new ExecuteCgraEvent(currentTime, selfIdx, i, cbid);
            cgra->pushToCgraQueue(event);
        }
    }
}

}
}
}