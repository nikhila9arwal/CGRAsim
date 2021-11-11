
#pragma once

#include "cgra.h"
#include "cgra_operation.h"

namespace cgra {

class ProcessingElement {
  public:
    ProcessingElement(PeIdx numPes, OpIdx numOps, CbIdx numThrds);
    // ~ProcessingElement();

    void loadBitstream(Config& bitstream, std::string prefix); 

    StrongVec<OpIdx, Operation> operations;
   
    // std::vector<Operation::Operands> tokenstore;
};

} // namespace cgra
