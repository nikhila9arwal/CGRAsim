
#include "cgra.h"
#include "cgra_network.h"
#include "cgra_pe.h"


namespace platy {
namespace sim {
namespace cgra {


Cgra::Cgra(
    // const std::string& name,
    // TileIdx tile,
    // EngineIdx _engIdx,
    uint32_t _numPes,
    uint32_t _numInstrsPerPE, 
    uint32_t _numThrds)
    {//: Engine(name, tile, _engIdx) {
    cbidx = 0_cbid;
    for (PeIdx p = 0_peid; p < (PeIdx)_numPes; p++) {
        processingElements.push_back(
            new ProcessingElement{_numInstrsPerPE * _numThrds, _numInstrsPerPE, p, this});
    }
    network = new BusNetwork(this);
    
    
    //Not sure about this
    currentTime = 0_cycles;
    networkDelay = 2_cycles;
    executionDelay = 1_cycles;
    setTokenDelay = 2_cycles;
    setTokenFailDelay = 1_cycles;
}

void Cgra::pushEvent(CgraEvent* event){
    pq.push(event);
}

Network* Cgra::getNetwork(){
    return network;
}

// TODO (nikhil): Change wunderpus decompression cfg to have params
void Cgra::configure(const FunctionConfiguration& functionConf){//const Engine::FunctionConfiguration& functionConf){
    // Config conf(functionConf.filename.c_str());
    Config conf(functionConf.filename.c_str());
    loadBitstream(conf);
    // loadStaticParams(conf, functionConf.context);
    loadStaticParams(conf, functionConf.context);
    loadInputMap(conf);
}

void Cgra::execute(std::shared_ptr<TaskReq> req){//std::shared_ptr<TaskReq> req) {
    // TODO (nikhil): check size of inputs

    // TODO (nikhil) :  Move to new function
    // TODO (nikhil): should be done in the operand class or somewhere else Idk.

    // initial check for what's ready because of immediates
    // for (PeIdx p = 0_peid; p < processingElements.size(); p++) {
    //     processingElements[p]->pushFullyImmediateInstructions(cbidx);
    // }

    // Word* runtimeInputs = (Word*)req->args;
    Word* runtimeInputs = (Word*)req->args;
    loadRuntimeInputs(runtimeInputs);
    cbidx = cbidx + 1_cbid;
}

void Cgra::tick() {
    if (pq.empty()) {
        throw OutOfEvents{};
    }
    
    // move time forward to the next event
    currentTime = pq.top()->timestamp;

    // execute until the next time step
    while (!pq.empty() && pq.top()->timestamp <= currentTime) {
        CgraEvent * event = pq.top();
        pq.pop();
        event->go(this);
    }
}

void Cgra::loadBitstream(Config& bitstream) {
    for (PeIdx i = 0_peid;; i++) {
        std::string configKey = qformat("pe_{}", i);
        if (!bitstream.exists(configKey)) {
            break;
        }
        qassert(i < processingElements.size());
        processingElements[i]->loadBitstream(bitstream, configKey);
    }
}

// 1.) Extract params offsets and
// 2.) Fetch params from context+offset
// 3.) Place them as as directed config destinations
void Cgra::loadStaticParams(Config& bitstream, void* context) {
    for (int i = 0;; i++) {
        std::string paramKey = qformat("param_{}", i);
        if (!bitstream.exists(paramKey)) {
            break;
        }
        int paramOffset = bitstream.get<int32_t>(paramKey + ".offset");
        Word param = *(Word*)((uint8_t*)context + paramOffset);
        for (int j = 0;; j++) {
            std::string paramDestKey = paramKey + qformat(".dest_{}", j);
            if(!bitstream.exists(paramDestKey)) break;
            auto loc = Location(bitstream, paramDestKey);
            processingElements[loc.pe]->setStaticParam(loc, param);
        }
    }
}

void Cgra::loadInputMap(Config& bitstream) {
    for (int i = 0;; i++) {
        std::string inputKey = qformat("input_{}", i);
        if (!bitstream.exists(inputKey)) {
            break;
        }
        std::vector<Location> destInput;
        for (int j = 0;; j++) {
            std::string destInputKey = inputKey + qformat(".dest_{}", j);
            if (bitstream.exists(destInputKey)) {
                auto loc = Location(bitstream, destInputKey);
                destInput.push_back(loc);
            } else {
                inputDestinationMap.push_back(destInput);
                break;
            }
        }
    }
}

// TODO (nikhil): change name to launch new thread return thread id
void Cgra::loadRuntimeInputs(Word* inputs) {
    for (auto destinations : inputDestinationMap) {
        for (auto destination : destinations) {
            TokenStore::Token tok(destination.pos, *inputs, destination.inst, cbidx);
            CgraEvent*  event = new SendTokenCgraEvent(currentTime, destination.pe, tok);
            pushEvent(event);
        }
        inputs++;
    }
}

}  // namespace cgra
}  // namespace sim
}  // namespace platy
