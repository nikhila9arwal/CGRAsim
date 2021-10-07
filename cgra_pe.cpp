#include "cgra_pe.h"

namespace cgra {

void ProcessingElement::loadBitstream(Config& bitstream, std::string prefix) {
    int numOperations;
    
    try {
        numOperations = conf.get<uint32_t>(prefix + ".operations", 1);
    } catch (/* ParserError ??? */ ...) {
        return;
    }

    numOutputs += numOperations;

    for (int j = 0; j < numOperations; j++) {
        operations[j].type = conf.get<const char*>("sys.tile_"+std::to_string(i)+".operation_"+std::to_string(j)+".type");;
            
        operations[j].decode();

        auto& ops=operations[j].operands;
            
        //TODO: set and use predicate and fallback
        ops.predicate.ready = true;
        ops.fallback.ready = true;
            
        ops.lhs.scalar = true;
        ops.lhs.tile = conf.get<int32_t>("sys.tile_"+std::to_string(i)+".operation_"+std::to_string(j)+".lhs.tile");
        ops.lhs.op = conf.get<int32_t>("sys.tile_"+std::to_string(i)+".operation_"+std::to_string(j)+".lhs.op");
        if(ops.lhs.tile < 0){
            ops.lhs.ready = true;
            ops.lhs.value[0] = inputs[ops.lhs.op];
        }
        else {
            ops.lhs.ready = false; 
        }

        //TODO : ready to a timestamp
        ops.rhs.scalar = true;
        ops.rhs.tile = conf.get<int32_t>("sys.tile_"+std::to_string(i)+".operation_"+std::to_string(j)+".rhs.tile");
        ops.rhs.op = conf.get<int32_t>("sys.tile_"+std::to_string(i)+".operation_"+std::to_string(j)+".rhs.op");
        if(ops.rhs.tile < 0){
            ops.rhs.ready = true;
            ops.rhs.value[0] = inputs[ops.rhs.op];
        }
        else {
            ops.rhs.ready = false; 
        }
        operations[j].output.ready = false;


    }
}

    

} // namespace cgra
