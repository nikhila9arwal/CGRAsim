
#pragma once

#include "strong_int.h"
#include "strong_vec.h"

namespace cgra {

DERIVE_STRONGER_INT(PeIdx, int32_t, peid)
DERIVE_STRONGER_INT(OpIdx, int32_t, opid)
DERIVE_STRONGER_INT(CallbackIdx, int32_t, cbid)

} // namespace cgra
