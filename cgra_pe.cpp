#include "cgra_pe.h"

namespace cgra {

ProcessingElement::ProcessingElement(OpIdx numOps){
    for(OpIdx i=0; i<numOps; i++) {
        operations.emplace_back(Operation{});
    }
}

void ProcessingElement::loadBitstream(Config& bitstream, std::string prefix) {
    for (int i=0; ; i++){
        std::string s = prefix + qformat(prefix+".operation_{}",i);
        if (bitstream.exists(s)) {
            assert(i<operations.size());
            operations[i].loadBitstream(bitstream,s)
        } else {
            break;
        }
    }
}

    

} // namespace cgra
