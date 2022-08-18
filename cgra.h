
#pragma once

#include "cgra_defs.h"
#include "cgra_event.h"

namespace platy {
namespace sim {
namespace cgra {

class Network;
class ProcessingElement;
class InputPort;

/**
 * @brief A single CGRA fabric. @brief
 */
class Cgra {
public:
    Cgra(
        // const std::string& name,
        // TileIdx tile,
        // EngineIdx _engIdx,
        uint32_t _numPes,
        uint32_t _numInsts,
        uint32_t _numThrds);

    void setCaches(BaseCache* _l1d);
    BaseCache* getL1d();
    // void configure(const FunctionConfiguration& functionConf);
    void configure(const platy::sim::ms::Engine::FunctionConfiguration& functionConf);
    
    void unconfigure(void* funcPtr);
    // void execute(std::shared_ptr<TaskReq> req);

    void execute(std::shared_ptr<platy::sim::ms::TaskReq> req);
    
    void pushEvent(CgraEvent* event, Cycles timestamp);

    class OutOfEvents {};
    
    void tick();

    // Cycles now() const;
    Word dload(Word readAddr);

    void store(Word writeAddr, Word val);

    Cycles nextEventTime () const;

    ProcessingElement* getProcessingElement(PeIdx peid);
    
    const ProcessingElement* getProcessingElement(PeIdx peid) const;

    StrongVec<PeIdx, std::unordered_set<InstrMemIdx>>& getInstFreeList();

    Network* getNetwork();

    PhysicalInstAddr translateVirtualInstAddr (VirtualInstAddr vAddr);

    void executionLoop();
    // ConfIdx getConfidx() {return confidx;}

private:
    Cycles currentTime;
    // BaseCache* l1i;
    BaseCache* l1d;
    std::priority_queue<std::pair<Cycles, CgraEvent*>, 
        std::vector<std::pair<Cycles, CgraEvent*>>, Cmprtr> eventQueue;
    std::unordered_map<VirtualInstAddr, PhysicalInstAddr, VirtualInstAddr::HashFn> vTable;
    std::unordered_map<CbIdx, void*> cbTable;
    CbIdx cbidx;
    // ConfIdx confidx;
    Network* network;
    StrongVec<PeIdx, ProcessingElement*> processingElements;
    std::unordered_map<void*, std::vector<std::vector<Location>>> inputDestinationMap;
    StrongVec<PeIdx, std::unordered_set<InstrMemIdx>> instFreeList;
    InputPort* inputPort;

    void loadBitstream(Config& bitstream, void* functionPtr);
    void loadStaticParams(Config& bitstream, void* context, void* functionPtr);
    void loadInputMap(Config& bitstream, void* functionPtr);
    void loadRuntimeInputs(Word* inputs);
    void instAllocator(Config& bitstream, void* functionPtr);
};

}  // namespace cgra
}  // namespace sim
}  // namespace platy
