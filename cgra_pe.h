
#pragma once

#include "cgra.h"

namespace cgra {

class ProcessingElement {
  public:
    ProcessingElement(OpIdx numOps);
    ~ProcessingElement();

    void loadBitstream(Config& bitstream, std::string prefix);    
    StrongVec<OpIdx, Operation> operations;
};

} // namespace cgra
