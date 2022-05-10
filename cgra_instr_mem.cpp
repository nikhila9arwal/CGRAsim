

#include "cgra_instr_mem.h"
#include "cgra_pe.h"

namespace platy {
namespace sim {
namespace cgra {

InstructionMemory::InstructionMemory(ProcessingElement * _pe, uint32_t size)
    :pe(_pe), instructionMemory{(InstrMemIdx)size} {
}

void InstructionMemory::loadBitstream(Config& bitstream, std::string key, void* functionPtr, Cgra *cgra) {
    auto hostPeid = PeIdx(pe->getId());
    // auto freeList = cgra->getInstFreeList()[hostPeid];
    for (InstrMemIdx i = 0_instid;; i++) {
        std::string configKey = key + qformat(".inst_{}", i);
        if (bitstream.exists(configKey)) {
            qassert(i < instructionMemory.size());
            auto vAddr = VirtualInstAddr{functionPtr, hostPeid, i};
            // printf("%ld, %d, %d \n", int64_t(functionPtr), int(hostPeid), int(i));
            auto pAddr = cgra->translateVirtualInstAddr(vAddr);
            auto pInst = pAddr.instidx;
            // printf("%d, %d \n", int(pAddr.peidx), int(pAddr.instidx));
            // auto it  = freeList.find(pInst);
            // cout<<*it<<"\n";
            // qassert(it!=freeList.end());
            instructionMemory[pInst].loadBitstream(bitstream, configKey, functionPtr, cgra);
            // freeList.erase(it);
        } else {
            break;
        }
    }
}

}
}
}
