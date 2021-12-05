#include "cgra.h"
#include "core_cgra.h"

int main(){
    cgra::CGRACore core(4,4,4);
    core.loadBitstream("outdataflow.cfg");

    Word inputs[3] = {1,2,3};
    core.loadInputs(inputs);
    core.loadInputs(inputs);
    core.execute();
    return 0;
}