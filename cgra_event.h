
#pragma once

#include "cgra_defs.h"
// #include "cgra_tok_st.h"

namespace platy {
namespace sim {
namespace cgra {

// class Cgra;
class CgraEvent {
public:
    CgraEvent() {}

    virtual void go() = 0;
    virtual void printInfo() = 0;

};
struct Cmprtr {
    bool operator()(const std::pair<Cycles, CgraEvent *> lhs,
                  const std::pair<Cycles, CgraEvent *> rhs) const {
        // <  ==> > for min heap
        return lhs.first > rhs.first;
    }
};

}  // namespace cgra
}  // namespace sim
}  // namespace platy
