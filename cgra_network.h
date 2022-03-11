
#pragma once

#include "cgra_defs.h"
#include "cgra.h"

namespace platy {
namespace sim {
namespace cgra {

class Network {
public:
    Network(Cgra* _cgra) : cgra(_cgra) {}
    // TODO (nikhil): make this templated
    virtual void sendToken(PeIdx pe, TokenStore::Token tok) = 0;

protected:
    Cgra* cgra;
};

class BusNetwork : public Network {
public:
    BusNetwork(Cgra* _cgra) : Network(_cgra) {}
    void sendToken(PeIdx peid, TokenStore::Token tok);
};

}
}
}
