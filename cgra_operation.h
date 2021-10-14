#pragma once

#include "cgra.h"

namespace cgra {

class Operation {
    // num OperationType {
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
public:

    Operation();

    struct Operand {
        bool ready;
        bool immediate; //fixed when bitstream loaded
        bool scalar;
        Value value;

        inline Word get(size_t i) const {
            assert(ready);
            if (scalar) {
                return value[0];
            } else {
                return value[i];
            }
        }

        inline void set(Word val) const {
            assert(!ready && !immediate);
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
        inline bool ready() const {
            return predicate.ready && fallback.ready && lhs.ready && rhs.ready;
        }
    };
    struct Output {
        bool ready;
        Word value; //TODO: use vector here
    };

    // TODO: This is effectively a pointer to an Operand. Can be deleted.
    struct Location {
        PeIdx pe;
        OpIdx op;
        bool pos; //0 for lhs, 1 for rhs
    // uint32_t cbid; TODO
    };

    Operands operands;
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
    int execute();
    };
};
