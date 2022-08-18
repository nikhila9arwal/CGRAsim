#pragma once
#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {

// class Cgra;

class Port {
public:
    Port(int width);
    ~Port() {}

    /* @brief Return the first time that this Port will be available.
     */
    Cycles grab(Cycles occupancy = 1_cycles, Cycles acquireDelay = 0_cycles) ;

    Cycles tryAcquire();

    Cycles acquire(Cycles acquireDelay = 0_cycles);

    void release(Cycles occupancy = 1_cycles);

    bool isAvailable();

private:
    std::priority_queue<Cycles, std::vector<Cycles>, std::greater<Cycles>> available;
    // Cgra* cgra;
};

}
}
}
