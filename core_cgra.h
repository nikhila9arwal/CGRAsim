
#pragma once

//Sequence : outgoing edge list, timing, vectors instead of manual, cbid, oop, static vs input, termination

// namespace platy {
// namespace sim {


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "config.h" //TODO: fix this
#include <array> 
#include <iostream>
#define LINE_SIZE 64

//nikhil: why tid at the end?
// DERIVE_STRONGER_INT(OpIdx, int32_t, opid)
// DERIVE_STRONGER_INT(CallbackIdx, int32_t, cbid)

class CGRACore {//: public Core {
public:
    // Assumptions:
    //
    // The CGRA bitstream will describe the placement of different
    // operations and their latency. The compiler will account for
    // the different PE types and their placement in the fabric
    // during mapping.

    //TODO: input config operations, routing

    CGRACore(const std::string& configFile);



    // The CGRA model does timing and functional modeling, so this
    // simulate() does essentially nothing after a callback is
    // initiated (the CGRA does everything itself).
    //TODO: ?
    // void simulate(const Instruction& instr) override;

    // Run everything in the CGRA for a given cycle.
    void tick();
    typedef uint32_t Word;

private:

    int numTiles;
    int numPEs;
    int numInputs;
    int numOutputs;
    static const size_t NUM_VALUE_ELEMENTS = LINE_SIZE / sizeof(Word);
    typedef std::array<Word, NUM_VALUE_ELEMENTS> Value;
    Word * inputs; //TODO: use Value (vector) here


    struct Operand {
        bool ready;
        // bool immediate;
        bool scalar;
        int32_t tile; //tile -1 means inputs
        int32_t op;
        Value value;

        inline Word get(size_t i) const {
            assert(ready);
            if (scalar) {
                return value[0];
            } else {
                return value[i];
            }
        }

        //TODO:?

        // inline void reset() {
        //     assert(ready);
        //     if (!immediate) {
        //         ready = false;
        //     }
        // }
    };

    struct Operation {
        // enum OperationType {
        //     NOP,
        //     NOT,
        //     AND,
        //     OR,
        //     XOR,
        //     ADD,
        //     SUBTRACT,
        //     MULTIPLY,
        //     DIVIDE,
        //     LESSTHAN,
        //     GREATERTHAN,
        //     EQUALTO,
        //     LOAD,
        //     STORE,
        //     SELECT,  // do we need this with fallback??? get rid of fallback???
        //     MERGE,   // do we need these?
        //     SPLIT,   // do we need these?
        //     PRINT
        // }type;


        
        struct Operands {
            Operand predicate, fallback, lhs, rhs;

            inline bool ready() const {
                return predicate.ready && fallback.ready && lhs.ready && rhs.ready;
            }
        };

        struct Output {
            bool ready;
            Word value; //TODO: use vector here
        };

        struct Location{
            uint32_t tile;
            uint32_t operation;
            // uint32_t cbid; TODO
        };

        std::string type;
        Operands operands;
        Output output;
        uint32_t dest_count;
        Location * dest; //TODO: use vector

        // TODO: callback id indexing
        // StrongVec<CallbackIdx, Operands> operands;
        // OperationType type;
        // bool scalar;
        // TileIdx dstPE;
        // OpIdx dstOp;

        CGRACore::Word (*ApplyFn)(CGRACore::Word lhs, CGRACore::Word rhs);
        void decode();
        //TODO: use callbackindex here?
        void execute();
    };

    struct ProcessingElement {
        int numOperations;
        Operation *  operations;
    };
    ProcessingElement * processingElements;
};

// }  // namespace sim
// }  // namespace platy
