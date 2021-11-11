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
        Operand predicate, fallback, lhs, rhs;

        Operands(){
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
        }
        inline bool ready() const {
            return predicate.ready && fallback.ready && lhs.ready && rhs.ready;
        }
    };
    struct Output {
        bool ready;
        Word value; //TODO: use vector here
        Output(){ready = false;}
    };

    // TODO: This is effectively a pointer to an Operand. Can be deleted.
    struct Location {
        PeIdx pe;
        OpIdx op;
        bool pos; //0 for lhs, 1 for rhs
        inline void loadBitstream(Config& bitstream, std::string prefix){
            if (bitstream.exists(prefix)){
                pe = (PeIdx)bitstream.get<int32_t>(prefix+".pe");
                op = (OpIdx)bitstream.get<int32_t>(prefix+".op");
                pos = (bool)bitstream.get<int32_t>(prefix+".pos");
            }
        }
    // uint32_t cbid; TODO
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
