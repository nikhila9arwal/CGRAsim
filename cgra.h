/** @file cgra.h
 *  @brief Contains the interface to the CGRA fabric.
 *
 *  The interface presented here is the only interface that should be exposed to the 
 *  application.
 *
 *  @author Nikhil Agarwal (nikhilag)
 *  @author Nathan Beckmann (beckmann) 
 *  @author Brian Schwedock (bschwedo)
 *  @author Bas Yoovidhya (piratacy)
 *  @bug Does not currently compile.
 */
#pragma once

// #include "../engine.h"
#include "cgra_defs.h"
#include "cgra_tok_st.h"
#include "cgra_event.h"

namespace platy {
namespace sim {
namespace cgra {

class Network;
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
 
    /**
     * @brief Configures the CGRA fabric based on provided config file and 
     * context pointer.
     *
     * @param functionConf Contains metadata for the function to be configured. Includes 
     * filename and context pointer.
     */
    void configure(const FunctionConfiguration& functionConf);

    // void unconfigure(void* funcPtr) {}
    // void execute(std::shared_ptr<TaskReq> req);

    // TODO (nikhil): pull out taskreq. Engine needs to know which function to execute or 
    // route inputs to

    /** 
     * @brief Begins execution of application on the current thread.
     *
     * @param req A shared pointer to task's metadata.
     */
    void execute(std::shared_ptr<TaskReq> req);
    
    /**
     * @brief Pushes a CgraEvent onto the priority queue.
     *
     * @param event The CgraEvent to push onto the priority queue.
     */
    void pushEvent(CgraEvent* event);

    class OutOfEvents {};
   
    /**
     * @brief Pops and execute events, moves time forward.
     */
    void tick();

    inline Cycles now() const { return currentTime; }

    ProcessingElement* getProcessingElement(PeIdx peid) {
        return processingElements.at(peid);
    }
    const ProcessingElement* getProcessingElement(PeIdx peid) const {
        return const_cast<Cgra*>(this)->getProcessingElement(peid);     
    }

    // TODO (by): Maybe the implementation should be in this file?
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
    std::priority_queue<CgraEvent*, std::vector<CgraEvent*>, Cmprtr> pq;
    CbIdx cbidx;
    Network* network;
    StrongVec<PeIdx, ProcessingElement*> processingElements;
    std::vector<std::vector<Location>> inputDestinationMap;

    /**
     * @brief Configures each instruction's operation type and destinations.
     *
     * Sets immediates, optypes, predicates, and destinations for instructions.
     * This function is only used in Cgra::configure.
     *
     * @param bitstream The target configuration to be loaded.
     */
    void loadBitstream(Config& bitstream);

    /**
     * @brief Loads information from the target bitstream used to index into the context 
     * pointer for static parameters and configures them into the CGRA.
     *
     * Only used in Cgra::configure.
     *
     * @param bitstream The configuration that specifies how static parameters are to be 
     * loaded.
     * @param context The pointer to the actual static parameters.
     */
    void loadStaticParams(Config& bitstream, void* context);

    /**
     * @brief Initialize and store a vector of destination for each input in the config 
     * file.
     *
     * Only used in Cgra::configure.
     *
     * @param bitstream The configuration to be loaded.
     */
    void loadInputMap(Config& bitstream);

    /**
     * @brief Loads in application's inputs and configures using input map for current 
     * thread's application.
     *
     * Only used in Cgra::execute.
     *
     * @param inputs The pointer to the first argument input to the application.
     */
    void loadRuntimeInputs(Word* inputs);
};

}  // namespace cgra
}  // namespace sim
}  // namespace platy
