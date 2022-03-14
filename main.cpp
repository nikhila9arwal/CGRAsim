// #include "cgra.h"
// #include "core_cgra.h"
#include "cgra.h"

// int main(){
//     cgra::CGRACore core(4,4,4);
//     core.loadBitstream("outdataflow.cfg");

//     Word inputs[3] = {1,2,3};
//     core.loadInputs(inputs);
//     core.loadInputs(inputs);
//     core.execute();
//     return 0;
// }

int main(){
    
    // Config conf("wunderpus-decompress.cfg");

    // Word data[8];
    // Word bases[1] = {1};
    // Word deltas[8] = {31,15,31,15,31,15,31,15};

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

    platy::sim::cgra::Cgra cgra(4,4,4);
    cgra.configure("outdataflow_copy.cfg", NULL);
    uint64_t args[3] = {1, 1,1};
    cgra.execute(args);

    try {
        while (true) {
            cgra.tick();
        }
    } catch (platy::sim::cgra::Cgra::OutOfEvents e) {
        // nothing
    }
    
    return 0;
}
