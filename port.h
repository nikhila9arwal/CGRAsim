#pragma once
#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {

class Cgra;

class Port {
public:
    Port(int width, Cgra* _cgra) : available(width, 0_cycles), cgra(_cgra) {}
    ~Port() {}

    /* @brief Return the first time that this Port will be available.
     */
    Cycles grab(Cycles occupancy) ;

    Cycles tryAcquire();

    Cycles acquire();

    void release(Cycles occupancy);

    bool isAvailable();

private:
    std::deque<Cycles> available;
    Cgra* cgra;
};

}
}
}
