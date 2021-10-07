#include "core_cgra.h"


//TODO : change tile to pe
//TODO : static vs input ports
//TODO : 1 operation executed per pe per cycle
// namespace platy {
// namespace sim {
CGRACore::Word nop(CGRACore::Word lhs, CGRACore::Word rhs){
    return lhs+rhs;
}

CGRACore::Word add(CGRACore::Word lhs, CGRACore::Word rhs){
    return lhs + rhs;
}

CGRACore::Word multiply(CGRACore::Word lhs, CGRACore::Word rhs){
    return lhs * rhs;
}


CGRACore::CGRACore(const std::string& configFile){
        
    
    Config conf(configFile.c_str());

    numInputs = conf.get<uint32_t>("sys.inputs.count",2);
    inputs = new Word[numInputs]; //TODO: use vector
    for (int i=0; i<numInputs; i++){
        inputs[i] = conf.get<Word>("sys.inputs.i_"+ std::to_string(i),0);        
    }

    numOutputs = 0; //TODO: change to a better termination condition
    numPEs = conf.get<uint32_t>("sys.tiles", 1);
    processingElements = new ProcessingElement[numPEs];
    for (int i=0; i<numPEs; i++){
        // std::string pePrefix = "sys.tile_"+std::to_string(i); TODO
        // processingElements[i].init(conf,pePrefix); TODO
        processingElements[i].numOperations = conf.get<uint32_t>("sys.tile_"+std::to_string(i)+".operations",1);

        numOutputs += processingElements[i].numOperations;
        processingElements[i].operations = new Operation[processingElements[i].numOperations];

        for (int j=0; j<processingElements[i].numOperations; j++){
            processingElements[i].operations[j].type = conf.get<const char*>("sys.tile_"+std::to_string(i)+".operation_"+std::to_string(j)+".type");;
            
            processingElements[i].operations[j].decode();

            auto& ops=processingElements[i].operations[j].operands;
            
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
            processingElements[i].operations[j].output.ready = false;


        }
    }

    

}


//TODO : timing info for network, processing etc.
void CGRACore::tick(){

    while(numOutputs>0){ //all outputs are calculated
        //network
        for (int i=0; i<numPEs; i++){
            for(int j=0; j<processingElements[i].numOperations;j++){
                Operation &op = processingElements[i].operations[j];
                //TODO: predicate fallback
                if (!op.operands.lhs.ready){
                    op.operands.lhs.ready =  processingElements[op.operands.lhs.tile].operations[op.operands.lhs.op].output.ready;
                    op.operands.lhs.value[0] =  processingElements[op.operands.lhs.tile].operations[op.operands.lhs.op].output.value;
                }
                if (!op.operands.rhs.ready){
                    op.operands.rhs.ready =  processingElements[op.operands.rhs.tile].operations[op.operands.rhs.op].output.ready;
                    op.operands.rhs.value[0] =  processingElements[op.operands.rhs.tile].operations[op.operands.rhs.op].output.value;
                }
            }
        }


        //TODO: each processing element should call the tick method separately whenre just goes off if it's operands are ready
        //TODO: global timestamp for the overall cgra core for each tile_op

        for (int i=0; i<numPEs; i++){
            for(int j=0; j<processingElements[i].numOperations;j++){
                if (!processingElements[i].operations[j].output.ready && processingElements[i].operations[j].operands.ready()){
                    processingElements[i].operations[j].execute();
                    std::cout<<"Tile: "<<i<<" Op: "<<j<<" Value: "<< processingElements[i].operations[j].output.value<<"\n";
                    numOutputs--;
                }
            }
        }
    }

}

void CGRACore::Operation::execute() {
    //TODO: use cbid
    // Operands& ops = operands[idx];
    assert(operands.ready());

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

    //TODO:maybe?!
    // writeback and cleanup
    // predicate.reset();
    // lhs.reset();
    // rhs.reset();

    // return output;
}

//TODO: multiple operations per element. route to dest

void CGRACore::Operation::decode() {
    if ( type == "NOP") ApplyFn = nop; 
    else if ( type == "ADD") ApplyFn = add; 
    else if ( type == "MULTIPLY") ApplyFn = multiply; 
}

// }  // namespace sim
// }  // namespace platy
