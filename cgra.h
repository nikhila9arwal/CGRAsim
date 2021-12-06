#pragma once


#include <array> 
#include <stdint.h>
#include "strong_int.h"
#include "strong_vec.h"
#include "config.h"
#include <assert.h>
#include "qassert.h"


#define LINE_SIZE 64
// namespace cgra{ 

DERIVE_STRONGER_INT(PeIdx, int32_t, peid)
DERIVE_STRONGER_INT(OpIdx, int32_t, opid)
DERIVE_STRONGER_INT(PosIdx, int32_t, posid)
DERIVE_STRONGER_INT(CbIdx, int32_t, cbid)

typedef int64_t Word;
static const size_t NUM_VALUE_ELEMENTS = LINE_SIZE / sizeof(Word);
typedef std::array<Word, NUM_VALUE_ELEMENTS> Value;

// TODO: This is effectively a pointer to an Operand. Can be deleted.
    struct Location {
        PeIdx pe;
        OpIdx op;
        PosIdx pos; //0 for lhs, 1 for rhs, 2 for trigger
        inline void loadBitstream(Config& bitstream, std::string prefix){
            if (bitstream.exists(prefix)){
                pe = (PeIdx)bitstream.get<int32_t>(prefix+".pe");
                op = (OpIdx)bitstream.get<int32_t>(prefix+".op");
                pos = (PosIdx)bitstream.get<int32_t>(prefix+".pos");
            }
        }
    };
// } // namespace cgra
