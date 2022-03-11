
#pragma once

// #include "../engine.h"
#include "cgra_defs.h"
#include "cgra_ts.h"
#include "cgra_event.h"

namespace platy {
namespace sim {
namespace cgra {

class Network;
class BusNetwork;
class ProcessingElement;

/**
 * @brief A single CGRA fabric.
 */
class Cgra {//: public Engine {
public:
    Cgra(
        // const std::string& name,
        // TileIdx tile,
        // EngineIdx _engIdx,
        uint32_t _numPes,
        uint32_t _numInsts,
        uint32_t _numThrds);

    // void setCaches(BaseCache* _l1i, BaseCache* _l1d) : l1i(_l1i), l1d(_l1d) {;}
    // BaseCache* getL1d() { return l1d; }
    // void configure(const FunctionConfiguration& functionConf);
    void configure(std::string filename, void* context);
    // void unconfigure(void* funcPtr) {}
    // void execute(std::shared_ptr<TaskReq> req);

    //TODO (nikhil): pull out taskreq. Engine needs to know which function to execute or route inputs to
    void execute(uint64_t* const args);
    // TODO (nikhil): use getProcessingElement instead;
    
    void pushEvent(CgraEvent* event);

    class OutOfEvents {};
    
    void tick();

    inline Cycles now() const { return currentTime; }

    ProcessingElement* getProcessingElement(PeIdx peid) {
        return processingElements.at(peid);
    }
    const ProcessingElement* getProcessingElement(PeIdx peid) const {
        return const_cast<Cgra*>(this)->getProcessingElement(peid);     
    }

    Network* getNetwork();

    // TODO (nzb): Non-const member fields of classes should _never_ be publicly exposed.
    // TODO (nzb): All times are of type Cycles.
    Cycles currentTime;
    Cycles networkDelay;
    Cycles executionDelay;
    Cycles setTokenDelay;
    Cycles setTokenFailDelay;

private:
    // BaseCache* l1i;
    // BaseCache* l1d;
    std::priority_queue<CgraEvent*> pq;
    CbIdx cbidx;
    Network* network;
    StrongVec<PeIdx, ProcessingElement*> processingElements;
    std::vector<std::vector<Location>> inputDestinationMap;

    void loadBitstream(Config& bitstream);
    void loadStaticParams(Config& bitstream, void* context);
    void loadInputMap(Config& bitstream);
    void loadRuntimeInputs(Word* inputs);
};

}  // namespace cgra
}  // namespace sim
}  // namespace platy
