#include "port.h"

namespace platy {
namespace sim {
namespace cgra {

Port::Port(int width){
    for (int i=0; i<width; i++) available.push(0_cycles);
}

Cycles Port::grab(Cycles occupancy, Cycles acquireDelay) {
    auto time = acquire(acquireDelay);
    release(time - events::now() + occupancy); // release occupancy cycles after acquireTime
    return time;
}


Cycles Port::acquire(Cycles acquireDelay) {
    qassert(isAvailable());
    auto time = std::max(events::now() + acquireDelay/* TODO (nzb): How to get this??? */, available.top());
    available.pop();
    return time;
}

Cycles Port::tryAcquire(){
    if(!isAvailable())
        return Cycles(-1);
    return acquire();
}

void Port::release(Cycles occupancy) {
    available.push(events::now() + occupancy);
}

bool Port::isAvailable() {
    return !available.empty();
}

}
}
}