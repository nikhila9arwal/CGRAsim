
#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {

uint32_t getCallbackFlags(ms::tako::CallbackType callbackType){
    switch (callbackType) {
        case ms::tako::CallbackType::ON_MISS:
            return MemReq::REQUESTED_BY_ON_MISS | MemReq::FROM_LOCAL_ENGINE;
        case ms::tako::CallbackType::ON_WRITEBACK:
            return MemReq::REQUESTED_BY_ON_WRITEBACK | MemReq::FROM_LOCAL_ENGINE;
        case ms::tako::CallbackType::ON_CLEAN_EVICTION:
            return MemReq::REQUESTED_BY_ON_CLEAN_EVICTION | MemReq::FROM_LOCAL_ENGINE;
        case ms::tako::CallbackType::ON_SHRINK:
            return MemReq::REQUESTED_BY_ON_SHRINK | MemReq::FROM_LOCAL_ENGINE;
        case ms::tako::CallbackType::NONE:
            // CallbackType::NONE means that the task is not a callback (e.g.,
            // core-triggered task).
            return MemReq::FROM_LOCAL_ENGINE;
        default:
            panic("Invalid CallbackType: {}.", callbackType);
    }
}

Location::Location(PeIdx _pe, InstrMemIdx _inst, PosIdx _pos)
    : pe(_pe), inst(_inst), pos(_pos) {}

Location::Location(Config& bitstream, std::string key) {
    loadBitstream(bitstream, key);
}

void Location::loadBitstream(Config& bitstream, std::string key) {
    qassert(bitstream.exists(key));
    pe = (PeIdx)bitstream.get<int32_t>(key + ".pe");
    inst = (InstrMemIdx)bitstream.get<int32_t>(key + ".inst");
    pos = (PosIdx)bitstream.get<int32_t>(key + ".pos");
}

VirtualInstAddr::VirtualInstAddr(void* _functionPtr, PeIdx _peidx, InstrMemIdx _instidx) 
    : functionPtr(_functionPtr), peidx(_peidx), instidx(_instidx) {}
bool VirtualInstAddr::operator==(const struct VirtualInstAddr& other) const {  // <  ==> > for min heap
    return functionPtr == other.functionPtr && peidx == other.peidx && instidx == other.instidx;
}
std::size_t VirtualInstAddr::HashFn::operator() (const VirtualInstAddr &node) const
{
    std::size_t h1 = std::hash<void*>()(node.functionPtr);
    std::size_t h2 = std::hash<PeIdx>()(node.peidx);
    std::size_t h3 = std::hash<InstrMemIdx>()(node.instidx);
    return (h1 ^ h2 ^ h3);
}

} // namespace cgra
} // namespace sim
} // namespace platy
