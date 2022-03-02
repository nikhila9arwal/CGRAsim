#include "core_cgra.h"


//TODO : change tile to pe
//TODO : static vs input ports
//TODO : 1 operation executed per pe per cycle
// namespace platy {
// namespace sim {

namespace cgra {

//TODO: eforce type
CGRACore::CGRACore(uint32_t numPes, uint32_t numOps, uint32_t numThrds) {
    networkDelay = 2;
    cbidx = 0_cbid;
    now = 0;
    for (PeIdx p = 0_peid; p < (PeIdx)numPes; p++) {
        // processingElements.emplace_back(ProcessingElement{(OpIdx)numOps});
        processingElements.push_back(ProcessingElement((PeIdx)numPes, (OpIdx)numOps, (CbIdx)numThrds));
    }
}

void CGRACore::loadBitstream(std::string bitstreamFilename) {

    Config conf(bitstreamFilename.c_str());
    // std::cout<<"here";


    loadBitstream(conf);
    loadInputMap(conf);
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

void CGRACore::loadInputMap(Config& inputConfig){
    for (int i=0; ; i++){
        std::string s = qformat("input_{}",i);
        if (inputConfig.exists(s)){
            // assert(i<numInputs);
            // Word val = inputConfig.get<int32_t>(s + ".value");                 
            std::vector<Location> tempV;
            for (int j=0; ;j++){
                if(inputConfig.exists(s + qformat(".dest_{}",j))){
                    Location tempL;
                    tempL.loadBitstream(inputConfig, s + qformat(".dest_{}",j));
                    tempV.push_back(tempL);
                }
                else{
                    inputMap.push_back(tempV);
                    break;
                }
            }
        }
        else{
            break;
        }
    }


}

//TODO: change name to launch new thread return thread id
void CGRACore::loadInputs(Word * input){

    //TODO:  check size of input vector

    //TODO: make readable j->destination
    for (unsigned int i=0; i<inputMap.size(); i++){
        for (unsigned int j=0; j<inputMap[i].size(); j++){
            if(inputMap[i][j].pos == 0_posid){
                processingElements[inputMap[i][j].pe].operations[inputMap[i][j].op].operands[cbidx].lhs.set(input[i]);
            }else if (inputMap[i][j].pos == 1_posid){
                processingElements[inputMap[i][j].pe].operations[inputMap[i][j].op].operands[cbidx].rhs.set(input[i]);
            }else if (inputMap[i][j].pos == 2_posid){
                processingElements[inputMap[i][j].pe].operations[inputMap[i][j].op].operands[cbidx].trigger = (bool)input[i];
            }
        }
    }

    //TODO: should be done in the operand class
    //initial check for what's ready
    for (PeIdx p = 0_peid; p < processingElements.size(); p++) {
        for (OpIdx o = 0_opid; o < processingElements[p].operations.size(); o++) {
            if(processingElements[p].operations[o].operands[cbidx].ready()) {
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
// TODO: Separate out routing and execution as separate tasks in the queue
void CGRACore::tick() {
    while(!pq.empty() && pq.top().timestamp <= now) {
        TimeSpace x = pq.top(); //TODO : anything but x
        pq.pop();

        // TODO: All of this should be in ProcessingElement::execute
        Operation &op = processingElements[x.pe].operations[x.op];
        op.execute(x.cb);
        std::cout<<now<<" "<<"thread"<<x.cb<<", pe"<<x.pe<<", op"<<x.op<<" : "<<op.output.value<<"\n";
        // if (op.output.ready) {
            for (uint32_t i = 0; i < op.dest.size(); i++) {
                // shouldn't the dest just be a Operand* ???
                auto &dest = processingElements[op.dest[i].pe].operations[op.dest[i].op];
                
                if (op.dest[i].pos == 0_posid) {
                    dest.operands[x.cb].lhs.set(op.output.value);
                } else if(op.dest[i].pos == 1_posid) {
                    dest.operands[x.cb].rhs.set(op.output.value);
                }
                else if(op.dest[i].pos == 2_posid){
                    dest.operands[x.cb].trigger = (bool) op.output.value;
                }
                
                if (dest.operands[x.cb].ready()) {
                    // TODO: Too much stuff happening in one line.
                    pq.push(TimeSpace(x.timestamp+op.executionDelay+networkDelay,op.dest[i].pe,op.dest[i].op,x.cb));
                }
            }
        // }
        
    }
}

//TODO: take time for how long to input for
void CGRACore::execute(){

    while (!pq.empty()) {
        now = pq.top().timestamp;
        tick();
    }
}

} //namespace cgra

// }  // namespace sim
// }  // namespace platy
