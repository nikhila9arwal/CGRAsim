#include "core_cgra.h"


//TODO : change tile to pe
//TODO : static vs input ports
//TODO : 1 operation executed per pe per cycle
// namespace platy {
// namespace sim {

namespace cgra {

//TODO: eforce type
CGRACore::CGRACore(uint32_t numPes, uint32_t numOps) {
    networkDelay = 2;
    cbidx = 0_cbid;
    now = 0;
    for (PeIdx p = 0_peid; p < (PeIdx)numPes; p++) {
        // processingElements.emplace_back(ProcessingElement{(OpIdx)numOps});
        processingElements.push_back(ProcessingElement((OpIdx)numOps));
    }
}

void CGRACore::loadBitstream(std::string bitstreamFilename) {

    Config conf(bitstreamFilename.c_str());
    // std::cout<<"here";


    loadBitstream(conf);
}

void CGRACore::loadBitstream(Config& bitstream) {
    // read inputs until there are none left
    for (PeIdx i = 0_peid; ; i++){
        std::string s = qformat("pe_{}",i);
        if (bitstream.exists(s)) {
            assert(i<processingElements.size());
            processingElements[i].loadBitstream(bitstream,s);
        } else {
            break;
        }
    }
}

void CGRACore::loadInputs(std::string inputFilename){
    Config conf(inputFilename.c_str());
    loadInputs(conf);
}

void CGRACore::loadInputs(Config& inputConfig){
    for (int i=0; ; i++){
        std::string s = qformat("input_{}",i);
        if (inputConfig.exists(s)){
            // assert(i<numInputs);
            Word val = inputConfig.get<int32_t>(s + ".value");                 
            for (int j=0; ;j++){
                if(inputConfig.exists(s + qformat(".dest_{}",j))){
                    PeIdx pe = (PeIdx)inputConfig.get<int32_t>(s + qformat(".dest_{}.pe",j));
                    OpIdx op = (OpIdx)inputConfig.get<int32_t>(s + qformat(".dest_{}.op",j));
                    bool pos = inputConfig.get<int32_t>(s + qformat(".dest_{}.pos",j));
                    if(pos)
                        processingElements[pe].operations[op].operands[cbidx].rhs.set(val);
                    else
                        processingElements[pe].operations[op].operands[cbidx].lhs.set(val);
                }
                else{
                    break;
                }
            }
        }
        else{
            break;
        }
    }
    //initial check for what's ready
    for (PeIdx p = 0_peid; p < processingElements.size(); p++) {
        for (OpIdx o = 0_opid; o < processingElements[p].operations.size(); o++) {
            if (processingElements[p].operations[o].operands[cbidx].ready()) {
                pq.push(TimeSpace{(int32_t)cbidx,p,o,cbidx});
            }
        }
    }

    cbidx = cbidx + 1_cbid;
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

// TODO: Re-write to call tick().
void CGRACore::tick() {
    while(!pq.empty() && pq.top().timestamp <= now) {
        TimeSpace x = pq.top();
        pq.pop();

        // TODO: All of this should be in ProcessingElement::execute
        Operation &op = processingElements[x.pe].operations[x.op];
        op.execute(x.cb);
        std::cout<<"thread"<<x.cb<<": "<<op.output.value<<"\n";
        // if (op.output.ready) {
            for (uint32_t i = 0; i < op.dest.size(); i++) {
                // shouldn't the dest just be a Operand* ???
                auto &dest = processingElements[op.dest[i].pe].operations[op.dest[i].op];
                
                if (op.dest[i].pos) {
                    dest.operands[x.cb].rhs.set(op.output.value);
                } else {
                    dest.operands[x.cb].lhs.set(op.output.value);
                }
                
                if (dest.operands[x.cb].ready()) {
                    // TODO: Too much stuff happening in one line.
                    pq.push(TimeSpace(x.timestamp+op.executionDelay+networkDelay,op.dest[i].pe,op.dest[i].op,x.cb));
                }
            }
        // }
        
    }
}

void CGRACore::execute(){

    while (!pq.empty()) {
        now = pq.top().timestamp;
        tick();
    }
}

} //namespace cgra

// }  // namespace sim
// }  // namespace platy
