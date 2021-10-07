
#pragma once

#include "cgra.h"

namespace cgra {

class ProcessingElement {
  public:
    ProcessingElement(uint32_t numOps);
    ~ProcessingElement();

    void loadBitstream(Config& bitstream, std::string prefix);

  private:
    StrongVec<OpIdx, Operation> operations;
};

} // namespace cgra
