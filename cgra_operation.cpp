#include "cgra_operation.h"

namespace cgra {

Operation::Operation(PeIdx numPes, OpIdx numOps, CbIdx numThrds){
    executionDelay = 1;
    for(CbIdx i = 0_cbid; i<numThrds; i++){
        operands.push_back(Operands());
    }

    //TODO: unused param error;
    numPes++;
    numOps++;
    // for (int32_t i=0; i< (int32_t)numPes*(int32_t)numOps*2; i++ ){
    //     dest.push_back(Location());
    // }
}

void Operation::loadBitstream(Config& bitstream, std::string prefix) {
    
    std::string type = bitstream.get<const char*>(prefix + ".type", "");
    qassert(type != "");
    decode(type);

    for(CbIdx i = 0_cbid; i<(CbIdx)operands.size(); i++){
        operands[i].loadBitstream(bitstream, prefix);
    }

    for (int i = 0; ; i++) {
        if(bitstream.exists(prefix+qformat(".dest_{}",i))) {
            dest.push_back(Location());
            dest[i].loadBitstream(bitstream,prefix+qformat(".dest_{}",i));
        } else{
            break;
        }
    }
}


//TODO: multiple operations per element. route to dest

void Operation::decode(std::string type) {
    if ( type == "NOP"){
        ApplyFn = [](Word lhs, Word rhs){return lhs + rhs;};
    }else if ( type == "NOT"){
        ApplyFn = [](Word lhs, Word rhs){return ~(lhs+rhs);};
    }else if ( type == "AND"){
        ApplyFn = [](Word lhs, Word rhs){return lhs & rhs;};
    }else if ( type == "OR"){
        ApplyFn = [](Word lhs, Word rhs){return lhs | rhs;};
    }else if ( type == "XOR"){
        ApplyFn = [](Word lhs, Word rhs){return lhs ^ rhs;};
    }else if ( type == "ADD"){
        ApplyFn = [](Word lhs, Word rhs){return lhs + rhs;};
    }else if ( type == "SUBTRACT"){
        ApplyFn = [](Word lhs, Word rhs){return lhs - rhs;};
    }else if ( type == "MULTIPLY"){
        ApplyFn = [](Word lhs, Word rhs){return lhs * rhs;};
    }else if ( type == "DIVIDE"){
        ApplyFn = [](Word lhs, Word rhs){return lhs / rhs;};
    }else if ( type == "LESSTHAN"){
        ApplyFn = [](Word lhs, Word rhs){return lhs < rhs ? 1:0;;};
    }else if ( type == "GREATERTHAN"){
        ApplyFn = [](Word lhs, Word rhs){return lhs > rhs ? 1:0;;};
    }else if ( type == "EQUALTO"){
        ApplyFn = [](Word lhs, Word rhs){return lhs == rhs ? 1:0;};
    }//else if ( type == "LOAD"){
    //     ApplyFn = [](Word lhs, Word rhs){return *(Word*)(lhs+rhs);};
    // }else if ( type == "STORE"){
    //     ApplyFn = [](Word lhs, Word rhs){*(Word*)lhs  = rhs; return 0;};
    // }else if ( type == "SELECT"){
    //     ApplyFn = [](Word lhs, Word rhs){if (lhs) return rhs; else return -1;};
    // }//else if ( type == "MERGE"){
    //     ApplyFn = [](Word lhs, Word rhs){return lhs * rhs;};
    // }else if ( type == "SPLIT"){
    //     ApplyFn = [](Word lhs, Word rhs){return lhs * rhs;};
    // }else if ( type == "PRINT"){
    //     ApplyFn = [](Word lhs, Word rhs){return lhs * rhs;};
    // }
    
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
