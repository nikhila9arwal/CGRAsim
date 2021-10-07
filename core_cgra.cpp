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

CGRACore::CGRACore(uint32_t numPes, uint32_t numOps) {
    for (PeIdx p = 0; p < (PeIdx)numPes; p++) {
        processingElements.emplace_back(ProcessingElement{(OpIdx)numOps});
    }
}

void CGRACore::loadBitstream(std::stream bitstreamFilename) {
    Config conf(configFile.c_str());

    loadBitstream(conf);
}

void CGRACore::loadBitstream(Config& bitstream) {
    // read inputs until there are none left
    for (int i = 0; ; i++) {
        try {
            Word input = bitstream.get<Word>(qformat("sys.inputs_{}", i));
            inputs.push_back(input);
        } catch (...) {
            break;
        }
    }

    numOutputs = 0; //TODO: change to a better termination condition
    for (PeIdx p = 0; p < processingElements.size(); p++) {
        processingElements[p].loadBitstream(bitstream, qformat("sys.pe_{}", p));
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
