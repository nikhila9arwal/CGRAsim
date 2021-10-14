
#pragma once

//Sequence : outgoing edge list, timing, vectors instead of manual, cbid, oop, static vs input, termination

// namespace platy {
// namespace sim {

//(t)input_ready->(t+e)output_ready->(t+e+n)set next_input
//break into two parts :
//1) if operands not ready then fetch data into inputs
//2) if operands ready execute


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "config.h" //TODO: fix this
#include <array> 
#include <iostream>
#include "cgra.h"
#include "cgra_pe.h"
#include "cgra_operation.h"

namespace cgra {

class CGRACore {//: public Core {
public:
    // Assumptions:
    //
    // The CGRA bitstream will describe the placement of different
    // operations and their latency. The compiler will account for
    // the different PE types and their placement in the fabric
    // during mapping.

    // TODO: routing
    CGRACore(uint32_t numPes, uint32_t numOps);
    ~CGRACore();

    void loadBitstream(std::string bitstreamFilename);
    void loadBitstream(Config& bitstream);
    void loadInputs(std::string inputFilename);
    void loadInputs(Config& inputConfig);

    // The CGRA model does timing and functional modeling, so this
    // simulate() does essentially nothing after a callback is
    // initiated (the CGRA does everything itself).
    //TODO: ?
    // void simulate(const Instruction& instr) override;

    // Run everything in the CGRA for a given cycle.
    // void tick();
    void execute();

private:

    // int numTiles;
    int numInputs;
    int networkDelay;
    // int numOutputs;
    // std::vector<Word> inputs;
    // Word * inputs; //TODO: use Value (vector) here
    struct TimeSpace{
        int32_t timestamp;
        PeIdx pe;
        OpIdx op;
        TimeSpace(int32_t timestamp, PeIdx pe, OpIdx op) : timestamp(timestamp), pe(pe), op(op)
        {
        }
        bool operator<(const struct TimeSpace& other) const // <  ==> > for min heap
        {
            return timestamp > other.timestamp;
        }
    }

    StrongVec<PeIdx, ProcessingElement> processingElements;
};

} // namespace cgra

// }  // namespace sim
// }  // namespace platy
