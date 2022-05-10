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

struct Simple1{
    int64_t * param;
};

int main(){
    

    WunderpusDecompression params;
    params.data = new int64_t[8];
    params.deltas = new int64_t[8] {31,15,31,15,0x5f,15,31,15};
    params.bases = new int64_t{1};

    platy::sim::cgra::FunctionConfiguration functionConfWunderpus;
    functionConfWunderpus.filename = "apps/wunderpus-decompress_single_PE.cfg";
    functionConfWunderpus.context = &params;
    functionConfWunderpus.isMemberFunction = false;
    functionConfWunderpus.functionPtr = &functionConfWunderpus.filename;

    uintptr_t args =  (uintptr_t)&(params.data[1]);


    Simple1 simpleParams;
    simpleParams.param =  new int64_t {4};
    platy::sim::cgra::FunctionConfiguration functionConfSimple1;
    functionConfSimple1.filename = "apps/simple1_single_pe.cfg";
    functionConfSimple1.context =  &simpleParams;
    functionConfSimple1.isMemberFunction = false;
    functionConfSimple1.functionPtr = &functionConfSimple1.filename;
    const uint64_t* simpleInput = new uint64_t[2] {2,0};



    platy::sim::cgra::Cgra cgra(/*pes=*/1,/*insts=*/16,/*threads=*/4);
    cgra.configure(functionConfWunderpus);
    cgra.configure(functionConfSimple1);

    auto req = std::make_shared<platy::sim::cgra::TaskReq>(0_pid, 0_tid, &functionConfWunderpus.filename, &args, sizeof(args));
    cgra.execute(req);


    req = std::make_shared<platy::sim::cgra::TaskReq>(0_pid, 0_tid, &functionConfSimple1.filename, simpleInput, 2*sizeof(uint64_t));
    cgra.execute(req);

    
    for (int i =0; i< 5; i++){
        cgra.tick();   
    }

    args =  (uintptr_t)&(params.data[4]);
    req = std::make_shared<platy::sim::cgra::TaskReq>(0_pid, 0_tid, &functionConfWunderpus.filename, &args, sizeof(args));

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
    std::cout<<*simpleParams.param<<"\n";

    
    return 0;
}
