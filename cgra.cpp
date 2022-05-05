
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
    uint32_t _numThrds) {   //: Engine(name, tile, _engIdx) {
    
    cbidx = 0_cbid;
    for (PeIdx p = 0_peid; p < (PeIdx)_numPes; p++) {
        processingElements.push_back(
            new ProcessingElement{_numInstrsPerPE* _numThrds, _numInstrsPerPE, p, this});
    }
    network = new BusNetwork(this, 8);
    inputPort = new InputPort(this, network);
    //Not sure about this
    currentTime = 0_cycles;

    std::unordered_set<InstrMemIdx> freeInsts;
    for (InstrMemIdx i=0_instid; i<InstrMemIdx(_numInstrsPerPE); i++) {freeInsts.insert(i);}
    instFreeList.insert(instFreeList.end(), PeIdx(_numPes), *new std::unordered_set<InstrMemIdx>(freeInsts));
    cout<<instFreeList[0_peid].size()<<"\n";

}

void Cgra::pushEvent(CgraEvent* event, Cycles timestamp){
    eventQueue.push(std::make_pair(timestamp, event));
}

Network* Cgra::getNetwork(){
    return network;
}

void Cgra::instAllocator(Config& bitstream, void* functionPtr){

    for (PeIdx i = 0_peid;; i++) {
        std::string peKey = qformat("pe_{}", i);
        if (!bitstream.exists(peKey)) { break; }
        qassert(i<processingElements.size());
        for (InstrMemIdx j = 0_instid;; j++) {
            std::string instKey = peKey + qformat(".inst_{}", j);
            if(!bitstream.exists(instKey)) { break; }
            qassert(!instFreeList[i].empty());
            auto firstFreeInst = instFreeList[i].begin();
            vTable[VirtualInstAddr{functionPtr, i, j}] = PhysicalInstAddr{i, *firstFreeInst};
            instFreeList[i].erase(firstFreeInst);
        }
    }
}


// TODO (nikhil): Change wunderpus decompression cfg to have params
void Cgra::configure(const FunctionConfiguration& functionConf){//const Engine::FunctionConfiguration& functionConf){
    // Config conf(functionConf.filename.c_str());
    Config conf(functionConf.filename.c_str());
    //cbid = scheduler(conf);
    //
    instAllocator(conf, functionConf.functionPtr);
    loadBitstream(conf, functionConf.functionPtr);
    // loadStaticParams(conf, functionConf.context);
    loadStaticParams(conf, functionConf.context, functionConf.functionPtr);
    loadInputMap(conf, functionConf.functionPtr);
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
    // if (inputsPendingAck>0){
    //     std::cout<<"Execution failed. Inputs pending from previous run. Try again.";
    // }
    Word* runtimeInputs = (Word*)req->args;
    cbTable[cbidx] = req->task;

    loadRuntimeInputs(runtimeInputs);

    cbidx = cbidx + 1_cbid;
}

void Cgra::tick() {
    if (eventQueue.empty()) {
        throw OutOfEvents{};
    }

    // execute until the next time step
    // pq elements are pair<Cycles, Event*>
    printf("\n\n------------------------------Cycle%d------------------------------\n\n",int(now()));
    while (!eventQueue.empty() && eventQueue.top().first <= currentTime) {
        CgraEvent* event = eventQueue.top().second;
        eventQueue.pop();
        event->printInfo();
        event->go();
    }
    // move time forward to the next event
    currentTime = eventQueue.top().first; // corresponds to time on top of queue.
}

void Cgra::loadBitstream(Config& bitstream, void* functionPtr) {
    for (PeIdx i = 0_peid;; i++) {
        std::string configKey = qformat("pe_{}", i);
        if (!bitstream.exists(configKey)) {
            break;
        }
        qassert(i < processingElements.size());
        processingElements[i]->loadBitstream(bitstream, configKey, functionPtr);
    }
}

// 1.) Extract params offsets and
// 2.) Fetch params from context+offset
// 3.) Place them as as directed config destinations
void Cgra::loadStaticParams(Config& bitstream, void* context, void* functionPtr ) {
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
            auto pAddr = translateVirtualInstAddr(VirtualInstAddr{functionPtr, loc.pe, loc.inst});
            loc.pe = pAddr.peidx;
            loc.inst = pAddr.instidx;
            processingElements[loc.pe]->setStaticParam(loc, param);
        }
    }
}

void Cgra::loadInputMap(Config& bitstream, void* functionPtr) {
    std::vector<std::vector<Location>> confInputDestinationMap;
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
                auto pAddr = translateVirtualInstAddr(VirtualInstAddr{functionPtr, loc.pe, loc.inst});
                loc.pe = pAddr.peidx;
                loc.inst = pAddr.instidx;
                destInput.push_back(loc);
            } else {
                confInputDestinationMap.push_back(destInput);
                break;
            }
        }
    }
    inputDestinationMap[cbTable[cbidx]] = confInputDestinationMap;
}

// TODO (nikhil): change name to launch new thread return thread id
void Cgra::loadRuntimeInputs(Word* inputs) {
    for (auto destinations : inputDestinationMap[cbTable[cbidx]]) {

        inputPort->acceptToken(*inputs, destinations, cbidx);
        inputs++;

#if 0
        for (auto destination : destinations) {
            TokenStore::Token tok(destination.pos, *inputs, destination.inst, cbidx);
            CgraEvent*  event = new SendTokenCgraEvent(currentTime, destination.pe, tok);
            pushEvent(event);
        }
        inputs++;
#endif
    }
}


}  // namespace cgra
}  // namespace sim
}  // namespace platy
