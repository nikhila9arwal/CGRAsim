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

// TODO:
Operation::Operation(/*CbIdx max*/){
    executionDelay = 1;
    for(CbIdx i = 0_cbid; i<8_cbid; i++){
        operands.push_back(Operands());
    }
}
void Operation::loadBitstream(Config& bitstream, std::string prefix) {
    
    std::string type = bitstream.get<const char*>(prefix + ".type", "");
    qassert(type != "");
    decode(type);

    // TODO: Operands should initialize themselves.
    // operands.lhs.loadBitstream(bitstream, prefix+".imm_lhs");
    if (bitstream.exists(prefix+".imm_lhs")){
        for(CbIdx i = 0_cbid; i<(CbIdx)operands.size(); i++){
            operands[i].lhs.value[0] = bitstream.get<int32_t>(prefix+".imm_lhs");
            operands[i].lhs.immediate = true;
            operands[i].lhs.scalar = true;
            operands[i].lhs.ready = true;
        }
    }else{
        for(CbIdx i = 0_cbid; i<(CbIdx)operands.size(); i++){
            operands[i].lhs.immediate = false;
            operands[i].lhs.scalar = true;
            operands[i].lhs.ready = false;
        }
    }

    if (bitstream.exists(prefix+".imm_rhs")){
        for(CbIdx i = 0_cbid; i<(CbIdx)operands.size(); i++){
            operands[i].rhs.value[0] = bitstream.get<int32_t>(prefix+".imm_rhs");
            operands[i].rhs.immediate = true;
            operands[i].rhs.scalar = true;
            operands[i].rhs.ready = true;
        }
    }else{
        for(CbIdx i = 0_cbid; i<(CbIdx)operands.size(); i++){
            operands[i].rhs.immediate = false;
            operands[i].rhs.scalar = true;
            operands[i].rhs.ready = false;
        }
    }
    
    for(CbIdx i = 0_cbid; i<(CbIdx)operands.size(); i++){

        operands[i].predicate.immediate = false;
        operands[i].predicate.scalar = true;
        operands[i].predicate.ready = true;
        
        operands[i].fallback.immediate = false;
        operands[i].fallback.scalar = true;
        operands[i].fallback.ready = true;

    }
    
    output.ready = false;

    for (int i = 0; ; i++) {
        // // TODO: Finish this...
        // if (bitstream.exists(...)) {
        //     break;
        // }
        if(bitstream.exists(prefix+qformat(".dest_{}",i))) {
            Location x;
            x.pe = (PeIdx)bitstream.get<int32_t>(prefix+qformat(".dest_{}.pe",i));
            x.op = (OpIdx)bitstream.get<int32_t>(prefix+qformat(".dest_{}.op",i));
            x.pos = bitstream.get<int32_t>(prefix+qformat(".dest_{}.pos",i));
            dest.push_back(x);
        } else{
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

int Operation::execute(CbIdx cbidx) {
    //TODO: use cbid
    // Operands& ops = operands[idx];
    // assert(operands.ready());

    if(!operands[cbidx].ready())
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

    output.value = ApplyFn(operands[cbidx].lhs.get(0), operands[cbidx].rhs.get(0));
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
