
#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {

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
