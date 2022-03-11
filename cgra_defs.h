
#pragma once

#include <array> 
#include <queue>
#include <string>
#include <map>
#include <stdint.h>
#include "strong_int.h"
#include "strong_vec.h"
#include "config.h"
#include <assert.h>
#include "qassert.h"
#include <memory>

// TODO (nzb): Remove when merged.
DERIVE_STRONGER_INT(Cycles, uint64_t, cycles)

DERIVE_STRONGER_INT(PeIdx, int32_t, peid)
DERIVE_STRONGER_INT(OpIdx, int32_t, opid)
DERIVE_STRONGER_INT(CbIdx, int32_t, cbid)
DERIVE_STRONGER_INT(InstrMemIdx, int32_t, instid)
DERIVE_STRONGER_INT(TokenStIdx, int32_t, tokenstid)

namespace platy {
namespace sim {
namespace cgra {

enum PosIdx { LHS, RHS, PREDICATE };
enum InstType {
    NOP,
    NOT,
    AND,
    OR,
    XOR,
    LSHIFT,
    RSHIFT,
    ADD,
    SUB,
    MUL,
    DIV,
    LESSTHAN,
    GREATERTHAN,
    EQUALTO,
    LOAD,
    STORE
};

typedef int64_t Word;  // TODO (nikhil): uint64_t maybe?
// static const size_t NUM_VALUE_ELEMENTS = LINE_SIZE / sizeof(Word);
// typedef std::array<Word, NUM_VALUE_ELEMENTS> Value;

// pass CGRA* to procElem so it can use these methods

struct Location {
    PeIdx pe;
    InstrMemIdx inst;
    PosIdx pos;  // TODO (nikhil): Use enum here -- 0 for lhs, 1 for rhs, 2 for predicate

    // TODO (nzb): Move implementation to cpp?
    Location(PeIdx _pe, InstrMemIdx _inst, PosIdx _pos);
    Location(Config& bitstream, std::string key);
    inline void loadBitstream(Config& bitstream, std::string key);
};

} // namespace cgra
} // namespace sim
} // namespace platy
