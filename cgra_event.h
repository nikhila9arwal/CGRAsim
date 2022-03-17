
#pragma once

#include "cgra_defs.h"
#include "cgra_tok_st.h"

namespace platy {
namespace sim {
namespace cgra {

class Cgra;
class CgraEvent {
public:
    CgraEvent(Cycles _timestamp) : timestamp(_timestamp) {}

    bool operator<(const class CgraEvent& other) const // <  ==> > for min heap
    {
        return timestamp > other.timestamp;
    }

    virtual void go(Cgra* cgra) const = 0;
    Cycles timestamp; // TODO (nzb): This doesn't need to be part of the event... you push events at a specific time instead
};

struct Cmprtr {
    bool operator()(const CgraEvent* first, const CgraEvent* second) const // <  ==> > for min heap
        {
            return first->timestamp > second->timestamp;
        }
};

}
}
}
