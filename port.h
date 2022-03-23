#pragma once
#include "cgra.h"
#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {


class Port {
public:
    Port(int width, Cgra* _cgra) : available(width, 0_cycles), cgra(_cgra) {}
    ~Port() {}

    /* @brief Return the first time that this Port will be available.
     */
    Cycles grab(Cycles occupancy = 1_cycles) {
        auto time = acquire();
        release(occupancy);
        return time;
    }

    Cycles acquire() {
        qassert(!empty());
        auto time = std::max(cgra->now() /* TODO (nzb): How to get this??? */, available.front());
        available.pop_front();
        return time;
    }

    void release(Cycles occupancy = 1_cycles) {
        available.push_back(cgra->now() + occupancy);
    }

    bool empty() {
        return available.empty();
    }

private:
    std::deque<Cycles> available;
    Cgra* cgra;
};

}
}
}
