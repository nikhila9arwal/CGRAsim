#include "port.h"
#include "cgra.h"

namespace platy {
namespace sim {
namespace cgra {

Cycles Port::grab(Cycles occupancy, Cycles acquireDelay) {
    auto time = acquire(acquireDelay);
    release(time - cgra->now() + occupancy); // release occupancy cycles after acquireTime
    return time;
}


Cycles Port::acquire(Cycles acquireDelay) {
    qassert(isAvailable());
    auto time = std::max(cgra->now() + acquireDelay/* TODO (nzb): How to get this??? */, available.top());
    available.pop();
    return time;
}

Cycles Port::tryAcquire(){
    if(!isAvailable())
        return Cycles(-1);
    return acquire();
}

void Port::release(Cycles occupancy) {
    available.push(cgra->now() + occupancy);
}

bool Port::isAvailable() {
    return !available.empty();
}

}
}
}