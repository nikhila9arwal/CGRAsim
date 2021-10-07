#include "core_cgra.h"

int main(){
    CGRACore core = CGRACore("dataflow.cfg");
    core.tick();
    return 0;
}