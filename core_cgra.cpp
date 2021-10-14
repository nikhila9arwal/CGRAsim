#include "core_cgra.h"


//TODO : change tile to pe
//TODO : static vs input ports
//TODO : 1 operation executed per pe per cycle
// namespace platy {
// namespace sim {

namespace cgra{ 
CGRACore::CGRACore(uint32_t numPes, uint32_t numOps) {
    networkDelay = 2;
    for (PeIdx p = 0; p < (PeIdx)numPes; p++) {
        processingElements.emplace_back(ProcessingElement{(OpIdx)numOps});
    }
}

void CGRACore::loadBitstream(std::stream bitstreamFilename) {
    Config conf(bitstreamFilename.c_str());

    loadBitstream(conf);
}

void CGRACore::loadBitstream(Config& bitstream) {
    // read inputs until there are none left
    numInputs = bitstream.get<int32_t>("numInputs");
    for (int i=0; ; i++){
        std::string s = qformat("pe_{}",i);
        if (bitstream.exists(s)){
            assert(i<processingElements.size());
            processingElements[i].loadBitstream(bitstream,s);
        }
        else{
            break;
        }
    }

    // for (int i = 0; ; i++) {
    //     try {
    //         Word input = bitstream.get<Word>(qformat("sys.inputs_{}", i));
    //         inputs.push_back(input);
    //     } catch (...) {
    //         break;
    //     }
    // }
}

void CGRACore::loadInputs(std::string inputFilename){
    Config conf(inputFilename.c_str());
    loadBitstream(conf);
}

void CGRACore::loadInputs(Config& inputConfig){
    for (int i=0; ; i++){
        std::string s = qformat("input_{}",i);
        if (inputConfig.exists(s)){
            assert(i<numInputs);
            Word val = inputConfig.get<int32_t>(s + ".value"));                 
            for (int j=0; ;j++){
                try{
                    PeIdx pe = inputConfig.get<int32_t>(s + qformat(".dest_{}.pe",j));
                    OpIdx op = inputConfig.get<int32_t>(s + qformat(".dest_{}.op",j));
                    bool pos = inputConfig.get<int32_t>(s + qformat(".dest_{}.pos",j));
                    if(pos)
                        processingElements[pe].operations[op].operands.rhs.set(val);
                    else
                        processingElements[pe].operations[op].operands.lhs.set(val);
                }
                catch(...){
                    break;
                }
            }
        }
        else{
            break;
        }
    }
}
        
// //TODO : timing info for network, processing etc.
// void CGRACore::tick(){

//     while(numOutputs>0){ //all outputs are calculated
//         //network
//         for (int i=0; i<numPEs; i++){
//             for(int j=0; j<processingElements[i].numOperations;j++){
//                 Operation &op = processingElements[i].operations[j];
//                 //TODO: predicate fallback
//                 if (!op.operands.lhs.ready){
//                     op.operands.lhs.ready =  processingElements[op.operands.lhs.tile].operations[op.operands.lhs.op].output.ready;
//                     op.operands.lhs.value[0] =  processingElements[op.operands.lhs.tile].operations[op.operands.lhs.op].output.value;
//                 }
//                 if (!op.operands.rhs.ready){
//                     op.operands.rhs.ready =  processingElements[op.operands.rhs.tile].operations[op.operands.rhs.op].output.ready;
//                     op.operands.rhs.value[0] =  processingElements[op.operands.rhs.tile].operations[op.operands.rhs.op].output.value;
//                 }
//             }
//         }


//         //TODO: each processing element should call the tick method separately whenre just goes off if it's operands are ready
//         //TODO: global timestamp for the overall cgra core for each tile_op

//         for (int i=0; i<numPEs; i++){
//             for(int j=0; j<processingElements[i].numOperations;j++){
//                 if (!processingElements[i].operations[j].output.ready && processingElements[i].operations[j].operands.ready()){
//                     processingElements[i].operations[j].execute();
//                     std::cout<<"Tile: "<<i<<" Op: "<<j<<" Value: "<< processingElements[i].operations[j].output.value<<"\n";
//                     numOutputs--;
//                 }
//             }
//         }
//     }

// }

void CGRACore::execute(){
    std::priority_queue<TimeSpace> pq; 

    //initial check for what's ready
    for (PeIdx p = 0; p < processingElements.size(); p++) {
        for (OpIdx o = 0; o<processingElements[p].size(); p++){
            if processingElements[p].operations[o].operands.ready()
                pq.push(TimeSpace(0,p,o));
        }
    }

    while(!pq.empty()){
        TimeSpace x = pq.top();
        pq.pop();
        Operation &op = processingElements[x.pe].operation[x.op];
        op.execute();
        if (op.output.ready){
            for (int i=0; i<op.dest.size(); op++){
                auto &dest = op.dest[i];
                if(dest.pos)
                    processingElements[dest.pe].operation[dest.op].operands.rhs.set(op.output.value);
                else
                    processingElements[dest.pe].operation[dest.op].operands.lhs.set(op.output.value);
                if(dest.operands.ready())
                    pq.push(TimeSpace(x.timestamp+op.executionDelay+networkDelay,dest.pe,dest.op));
            }
        }
        
    }

}
} //namespace cgra
// }  // namespace sim
// }  // namespace platy
