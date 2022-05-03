

#include "cgra_instr_mem.h"
#include "cgra_pe.h"

namespace platy {
namespace sim {
namespace cgra {

InstructionMemory::InstructionMemory(Cgra* _cgra, ProcessingElement * _pe, uint32_t size)
    : cgra(_cgra), pe(_pe), instructionMemory{(InstrMemIdx)size} {
}

void InstructionMemory::loadBitstream(Config& bitstream, std::string key) {
    auto hostPeid = PeIdx(pe->getId());
    auto freeList = cgra->getInstFreeList()[hostPeid];
    auto confId = cgra->getConfidx();
    for (InstrMemIdx i = 0_instid;; i++) {
        std::string configKey = key + qformat(".inst_{}", i);
        if (bitstream.exists(configKey)) {
            qassert(i < instructionMemory.size());
            auto vAddr = VirtualAddr{confId, hostPeid, i};
            auto pInst = cgra->getPAddr(vAddr).instidx;
            auto it  = freeList.find(pInst);
            qassert(it!=freeList.end());
            instructionMemory[pInst].loadBitstream(bitstream, configKey);
            freeList.erase(it);
        } else {
            break;
        }
    }
}

}
}
}
