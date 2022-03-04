
#pragma once

#include <array> 
#include <queue>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include "strong_int.h"
#include "strong_vec.h"
#include "config.h"
#include <assert.h>
#include "qassert.h"

#include "engine_cgra_event.h"
// #include "engine_cgra_pe.h"
// #include "engine_cgra_ts.h"
// #include "engine_cgra_im.h"
// #include "engine_cgra_network.h"
// #include "engine_cgra_event.h"



DERIVE_STRONGER_INT(PeIdx, int32_t, peid)
DERIVE_STRONGER_INT(OpIdx, int32_t, opid)
DERIVE_STRONGER_INT(CbIdx, int32_t, cbid)
DERIVE_STRONGER_INT(InstrMemIdx, int32_t, instid)
DERIVE_STRONGER_INT(TokenStIdx, int32_t, tokenstid)


namespace platy {
namespace sim {
namespace ms {

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

// TODO (nikhil) :  this becomes the CGRA class
// CGRA method has methods to addEventToQueue
// pass CGRA* to procElem so it can use these methods

uint32_t currentTime = 0;
uint32_t networkDelay = 2;
uint32_t executionDelay = 1;
uint32_t setTokenDelay = 2;
uint32_t setTokenFailDelay = 1;


std::priority_queue<CgraEvent> pq;


struct Location {
    PeIdx pe;
    InstrMemIdx inst;
    PosIdx pos;  // TODO (nikhil): Use enum here -- 0 for lhs, 1 for rhs, 2 for predicate
    Location(PeIdx _pe, InstrMemIdx _inst, PosIdx _pos)
        : pe(_pe), inst(_inst), pos(_pos) {}
    Location(Config& bitstream, std::string key) { loadBitstream(bitstream, key); }
    inline void loadBitstream(Config& bitstream, std::string key) {
        qassert(bitstream.exists(key));
        pe = (PeIdx)bitstream.get<int32_t>(key + ".pe");
        // TODO (nikhil) : change op -> inst
        inst = (InstrMemIdx)bitstream.get<int32_t>(key + ".inst");
        pos = (PosIdx)bitstream.get<int32_t>(key + ".pos");
    }
};

}
}
}