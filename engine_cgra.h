
#pragma once

// #include "../engine.h"
#include "engine_cgra_defs.h"
#include "engine_cgra_pe.h"
#include "engine_cgra_ts.h"
#include "engine_cgra_network.h"
#include "engine_cgra_event.h"

namespace platy {
namespace sim {
namespace ms {

class CgraEngine {//: public Engine {
public:
    CgraEngine(
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
    void execute(uint64_t* const args);
    void sendToken(PeIdx pe, TokenStore::Token tok) { network->sendToken(pe, tok); }
    bool setToken(PeIdx pe, TokenStore::Token tok) {
        return processingElements[pe].setToken(tok);
    }
    void executeInstruction(PeIdx pe, TokenStore::Tag tag) {
        processingElements[pe].executeInstruction(tag);
    }
    void pushToCgraQueue(CgraEvent* event){
        pq.push(event);
    }

private:
    // BaseCache* l1i;
    // BaseCache* l1d;
    std::priority_queue<CgraEvent*> pq;
    CbIdx cbidx;
    BusNetwork* network;
    StrongVec<PeIdx, ProcessingElement> processingElements;
    std::vector<std::vector<Location>> inputDestinationMap;

    void loadBitstream(Config& bitstream);
    void loadStaticParams(Config& bitstream, void* context);
    void loadInputMap(Config& bitstream);
    void loadRuntimeInputs(Word* inputs);
    void tick();
};

}  // namespace ms
}  // namespace sim
}  // namespace platy