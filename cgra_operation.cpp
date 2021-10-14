#include "cgra_operation.h"

namespace cgra {

// TODO: Let's do the fancy lambda shit.
Word nop(Word lhs, Word rhs){
    return lhs+rhs;
}

Word add(Word lhs, Word rhs){
    return lhs + rhs;
}

Word multiply(Word lhs, Word rhs){
    return lhs * rhs;
}

Operation::Operation() {
    executionDelay = 1;
}

void Operation::loadBitstream(Config& bitstream, std::string prefix) {
    
    std::string type = bitstream.get<const char*>(prefix + ".type", "");
    qassert(type != "", "Was expecting a type for {}", prefix);
    decode(type);

    // TODO: Operands should initialize themselves.
    operands.lhs.loadBitstream(bitstream, prefix+".imm_lhs");
    try {
        operands.lhs.value[0] = bitstream.get<int32_t>(prefix+".imm_lhs");
        operands.lhs.immediate = true;
        operands.lhs.scalar = true;
        operands.lhs.ready = true;
    }
    catch (...) {
        operands.lhs.immediate = false;
        operands.lhs.scalar = true;
        operands.lhs.ready = false;
    }

    try {
        operands.rhs.value[0] = bitstream.get<int32_t>(prefix+".imm_rhs");
        operands.rhs.immediate = true;
        operands.rhs.scalar = true;
        operands.rhs.ready = true;
    }
    catch (...) {
        operands.rhs.immediate = false;
        operands.rhs.scalar = true;
        operands.rhs.ready = false;
    }

    operands.predicate.immediate = false;
    operands.predicate.scalar = true;
    operands.predicate.ready = true;

    operands.fallback.immediate = false;
    operands.fallback.scalar = true;
    operands.fallback.ready = true;

    output.ready = false;

    for (int i = 0; ; i++) {
        // TODO: Finish this...
        if (bitstream.exists(...)) {
            break;
        }
        try {
            Location x;
            x.pe = bitstream.get<int32_t>(qformat(prefix+".dest_{}.pe",i));
            x.op = bitstream.get<int32_t>(qformat(prefix+".dest_{}.op",i));
            x.pos = bitstream.get<int32_t>(qformat(prefix+".dest_{}.pos",i));
            dest.push_back(x);
        } catch (...) {
            break;
        }
    }
}


//TODO: multiple operations per element. route to dest

void Operation::decode(std::string type) {
    // TODO: Fancy lambda shit.
    if ( type == "NOP") { ApplyFn = nop; }
    else if ( type == "ADD") { ApplyFn = add; }
    else if ( type == "MULTIPLY") { ApplyFn = multiply; }
} 

int Operation::execute() {
    //TODO: use cbid
    // Operands& ops = operands[idx];
    // assert(operands.ready());

    if(!operands.ready())
        return 1;
    
    // decode
    // CGRACore::Word (*apply)(CGRACore::Word lhs, CGRACore::Word rhs) = (void*)decode(); //TODO: move this in constructor to fix it to a given operation


    //TODO: Use predicate and use vector

    // size_t n = scalar ? 1 : NUM_VALUE_ELEMENTS;

    // for (size_t i = 0; i < n; i++) {
    //     if (predicate.get(i)) {
    //         output[i] = apply(lhs.get(i), rhs.get(i));
    //     } else {
    //         output[i] = fallback.get(i);
    //     }
    // }

    output.value = ApplyFn(operands.lhs.get(0), operands.rhs.get(0));
    output.ready = true;

    return 0;
    //TODO:maybe?!
    // writeback and cleanup
    // predicate.reset();
    // lhs.reset();
    // rhs.reset();

    // return output;
}   

} // namespace cgra
