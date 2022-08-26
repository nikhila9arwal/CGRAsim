
#include "cgra.h"
#include "cgra_network.h"
#include "cgra_pe.h"
#include "cgra_input_port.h"


#define DEBUG_CGRA
#ifdef DEBUG_CGRA
#define DBG(...) qlog(__VA_ARGS__)
#else
#define DBG(...)
#endif


namespace platy {
namespace sim {
namespace cgra {


Cgra::Cgra(
    // const std::string& name,
    // TileIdx tile,
    uint32_t _numPes,
    uint32_t _numInstrsPerPE, 
    uint32_t _numThrds) {   //: Engine(name, tile, _engIdx) {
    
    cbidx = 0_cbid;
    for (PeIdx p = 0_peid; p < (PeIdx)_numPes; p++) {
        processingElements.push_back(
            new ProcessingElement{_numInstrsPerPE* _numThrds, _numInstrsPerPE, p, this});
    }
    network = new BusNetwork(this, 8);
    inputPort = new InputPort(network);
    currentTime = 0_cycles;

    std::unordered_set<InstrMemIdx> freeInsts;
    for (InstrMemIdx i=0_instid; i<InstrMemIdx(_numInstrsPerPE); i++) {freeInsts.insert(i);}
    instFreeList.insert(instFreeList.end(), PeIdx(_numPes), std::unordered_set<InstrMemIdx>(freeInsts));

}

void Cgra::setCaches(BaseCache* _l1d){
    l1d = _l1d;
}

BaseCache* Cgra::getL1d() { 
    return l1d; 
}


void Cgra::unconfigure(void* funcPtr) {
    qassert(funcPtr!=nullptr);
}

void Cgra::pushEvent(CgraEvent* event, Cycles timestamp){
    Cycles oldTime = nextEventTime();
    eventQueue.push(std::make_pair(timestamp, event));

    if (nextEventTime() < oldTime) {spawn_event([&](){ 
        time::ff(nextEventTime());
        executionLoop(); 
    });}
}

Network* Cgra::getNetwork(){
    return network;
}

// Cycles Cgra::now() const { 
//     return events::now(); 
// }

Cycles Cgra::nextEventTime() const{
    if (eventQueue.empty()){
        return std::numeric_limits<Cycles>::max();
    } 
    return eventQueue.top().first;
}

ProcessingElement* Cgra::getProcessingElement(PeIdx peid) {
    return processingElements.at(peid);
}

const ProcessingElement* Cgra::getProcessingElement(PeIdx peid) const {
    return const_cast<Cgra*>(this)->getProcessingElement(peid);     
}

StrongVec<PeIdx, std::unordered_set<InstrMemIdx>>& Cgra::getInstFreeList() { 
    return instFreeList;
}

PhysicalInstAddr Cgra::translateVirtualInstAddr (VirtualInstAddr vAddr) { 
    return vTable[vAddr] ;
}


Word Cgra::dload(Word readAddr) {  

    auto dummyState = MESIState::I;
    uint32_t flags = 0;

    MemReq req = {
        LineAddr{ (ByteAddr) readAddr, 1_pid }, AccessType::GETS, 0_childIdx, MESIState::I, &dummyState,
        nullptr, LineIdx::INVALID, ByteAddr(-1ul), flags,
    };

    l1d->access(req);

    Word readVal;
    readFromApp<Word>(1_pid, (void*)readAddr, readVal);

    return readVal;  
}

void Cgra::store(Word writeAddr, Word val) {

    auto dummyState = MESIState::I;
    uint32_t flags = 0;

    MemReq req = {
        LineAddr{ ByteAddr(writeAddr), 1_pid }, AccessType::GETX, 0_childIdx, MESIState::I, &dummyState,
        nullptr, LineIdx::INVALID, ByteAddr(-1ul), flags,
    };
    l1d->access(req);

    writeToApp<Word>(1_pid, (void*)writeAddr, val);
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


void Cgra::configure(const platy::sim::ms::Engine::FunctionConfiguration& functionConf){//const Engine::FunctionConfiguration& functionConf){
    Config conf(functionConf.filename.c_str());
    instAllocator(conf, functionConf.functionPtr);
    loadBitstream(conf, functionConf.functionPtr);
    loadStaticParams(conf, functionConf.context, functionConf.functionPtr);
    loadInputMap(conf, functionConf.functionPtr);
}


void Cgra::endCallback(CbIdx cbid){
    DBG("Ending Callback {}", cbid);
    // qassert(callbackCvs[cbid].size() == 1);
    cbTable[cbid]->end();
}

void Cgra::execute(std::shared_ptr<platy::sim::ms::TaskReq> req){//std::shared_ptr<TaskReq> req) {

    Word* runtimeInputs = (Word*)req->args;
    cbTable[cbidx] = req->task;

    loadRuntimeInputs(runtimeInputs);

    cbidx = cbidx + 1_cbid;
}

//nikhil (todo): what if you have 2 threads that arrive here at the same time.

void Cgra::executionLoop(){
//nikhil: not clear whats going on here
    while(nextEventTime() == events::now()){
        tick(); // catch if infinity
        time::ff(nextEventTime()); //yield to simulator till cgra-> now()
    }
    qassert(nextEventTime() > events::now());
}

void Cgra::tick() {
    if (eventQueue.empty()) {
        throw OutOfEvents{};
    }

    currentTime = eventQueue.top().first; // corresponds to time on top of queue.

    DBG("\n\n------------------------------Cycle{}------------------------------\n\n", uint64_t(events::now()));
    
    while (!eventQueue.empty() && eventQueue.top().first <= currentTime) {
        CgraEvent* event = eventQueue.top().second;
        eventQueue.pop();
        event->printInfo();
        event->go();
    }
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
    inputDestinationMap[functionPtr] = confInputDestinationMap;
}

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
