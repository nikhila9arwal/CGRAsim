#include "cgra.h"
#include "core_cgra.h"

int main(){
    cgra::CGRACore core(4,4,4);
    core.loadBitstream("outdataflow.cfg");
    core.loadInputs("outdataflow.cfg");
    core.loadInputs("outdataflow.cfg");
    core.execute();
    return 0;
}