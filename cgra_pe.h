
#pragma once

#include "cgra.h"
#include "cgra_operation.h"

namespace cgra {

class ProcessingElement {
  public:
    ProcessingElement(OpIdx numOps);
    // ~ProcessingElement();

    void loadBitstream(Config& bitstream, std::string prefix);    
    StrongVec<OpIdx, Operation> operations;
};

} // namespace cgra
