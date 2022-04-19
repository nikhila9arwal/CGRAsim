// #include "cgra.h"
// #include "core_cgra.h"
#include "cgra.h"
#include <iomanip>
// #include "cgra_defs.h"

struct WunderpusDecompression{
    int64_t* data;
    int64_t* deltas;
    int64_t* bases;
};
int main(){
    

    // Word inputs1[4] = {(Word)data,(Word)data,(Word)deltas,(Word)bases};
    // Word inputs2[4] = {(Word)&data[1],(Word)data,(Word)deltas,(Word)bases};

    // cgra::CGRACore core(8,8,8);
    // core.loadBitstream("wunderpus-decompress.cfg"); //TODO: also take in dynamically determined arguments like deltas and bases instead of making them part of inputs.
    // core.loadInputs(inputs1);
    // core.loadInputs(inputs2);
    // core.execute();


    // std::cout<<data[0]<<"\n";
    // std::cout<<data[1]<<"\n";
    // return 0;

    WunderpusDecompression params;
    params.data = new int64_t[8];
    params.deltas = new int64_t[8] {31,15,31,15,0x5f,15,31,15};
    params.bases = new int64_t{1};

    platy::sim::cgra::FunctionConfiguration functionConf;
    functionConf.filename = "wunderpus-decompress_single_PE.cfg";
    functionConf.context = &params;
    functionConf.isMemberFunction = false;
    functionConf.functionPtr = nullptr;

    uintptr_t args =  (uintptr_t)&(params.data[1]);

    auto req = std::make_shared<platy::sim::cgra::TaskReq>(0_pid, 0_tid, nullptr, &args, sizeof(args));


    platy::sim::cgra::Cgra cgra(/*pes=*/1,/*insts=*/10,/*threads=*/2);
    cgra.configure(functionConf);
    cgra.execute(req);
    
    for (int i =0; i< 10; i++){
    cgra.tick();
        
    }


    args =  (uintptr_t)&(params.data[4]);
    req = std::make_shared<platy::sim::cgra::TaskReq>(0_pid, 0_tid, nullptr, &args, sizeof(args));

    cgra.execute(req);

    try {
        while (true) {
            cgra.tick();
        }
    } catch (platy::sim::cgra::Cgra::OutOfEvents e) {
        // nothing
    }
    std::cout<<"x"<<std::hex<<params.data[1]<<"\n";
    std::cout<<"x"<<std::hex<<params.data[4]<<"\n";

    
    return 0;
    // platy::sim::cgra::FunctionConfiguration functionConf;
    // functionConf.filename = "outdataflow_copy.cfg";
    // functionConf.context = nullptr;
    // functionConf.isMemberFunction = false;
    // functionConf.functionPtr = nullptr;

    // uint64_t* args = new uint64_t[3];
    // uint32_t argBytes = sizeof(uint64_t)*3;
    // args[0] = 2;
    // args[1] = 3;
    // args[2] = 4;
    // auto req = std::make_shared<platy::sim::cgra::TaskReq>(0_pid, 0_tid, nullptr, args, argBytes);



    // platy::sim::cgra::Cgra cgra(4,4,4);
    // cgra.configure(functionConf);
    // cgra.execute(req);

    // try {
    //     while (true) {
    //         cgra.tick();
    //     }
    // } catch (platy::sim::cgra::Cgra::OutOfEvents e) {
    //     // nothing
    // }
    
    // return 0;
}
