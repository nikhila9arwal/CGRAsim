#pragma once

#include "cgra.h"

namespace cgra {

class Operation {
public:

    Operation(PeIdx numPes, OpIdx numOps, CbIdx numThrds);
    // ~Operation();

    struct Operand {
        bool ready;
        bool immediate; //fixed when bitstream loaded
        bool scalar;
        Value value;

        Operand(){
            ready = false;
            immediate = false;
            scalar = true;
        }

        inline Word get(size_t i) {
            assert(ready);
            if (scalar) {
                return value[0];
            } else {
                return value[i];
            }
        }

        inline void set(Word val) {
            assert(!ready);
            assert(!immediate);
            if (scalar) {
                value[0] = val;
                ready = true;
            } else {
                return;
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
    
    struct Operands {
        bool trigger;
        Operand predicate, fallback, lhs, rhs;

        Operands(){
            trigger = true;
            predicate.ready = true;
            fallback.ready = true;
        }
        inline void loadBitstream(Config& bitstream, std::string prefix){
            if (bitstream.exists(prefix+".imm_lhs")){
                lhs.value[0] = bitstream.get<int32_t>(prefix+".imm_lhs");
                lhs.ready = true;
                lhs.immediate = true;
            }
            if (bitstream.exists(prefix+".imm_rhs")){
                rhs.value[0] = bitstream.get<int32_t>(prefix+".imm_rhs");
                rhs.ready = true;
                rhs.immediate = true;
            }
            if (bitstream.exists(prefix+".trigger")){
                trigger = (bool)bitstream.get<int32_t>(prefix+".trigger");
            }
        }
        inline bool ready() const {
            return trigger && predicate.ready && fallback.ready && lhs.ready && rhs.ready;
        }
    };
    struct Output {
        bool ready;
        Word value; //TODO: use vector here
        Output(){ready = false;}
    };

    // Operands operands;
    StrongVec<CbIdx, Operands> operands;
    Output output;
    std::vector<Location> dest;
    Word (*ApplyFn)(Word lhs, Word rhs);
    int executionDelay;
    // TODO: callback id indexing
    // StrongVec<CallbackIdx, Operands> operands;
    // OperationType type;
    // bool scalar;
    // TileIdx dstPE;
    // OpIdx dstOp;
    void loadBitstream(Config& bitstream, std::string prefix);
    void decode(std::string type);
    //TODO: use callbackindex here?
    int execute(CbIdx cbidx);
};

} // namespace cgra
