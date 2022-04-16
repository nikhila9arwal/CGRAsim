#include "port.h"
#include "cgra.h"

namespace platy {
namespace sim {
namespace cgra {

Cycles Port::grab(Cycles occupancy = 1_cycles) {
    auto time = acquire();
    release(occupancy);
    return time;
}

Cycles Port::acquire() {
    qassert(isAvailable());
    auto time = std::max(cgra->now() /* TODO (nzb): How to get this??? */, available.front());
    available.pop_front();
    return time;
}

void Port::release(Cycles occupancy = 1_cycles) {
    available.push_back(cgra->now() + occupancy);
}

bool Port::isAvailable() {
    return !available.empty();
}

}
}
}