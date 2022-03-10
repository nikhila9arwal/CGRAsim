
#pragma once

#include "engine_cgra_defs.h"
#include "engine_cgra.h"

namespace platy {
namespace sim {
namespace ms {

class Network {
public:
    Network(CgraEngine* _cgra) : cgra(_cgra) {}
    // TODO (nikhil): make this templated
    virtual void sendToken(PeIdx pe, TokenStore::Token tok) = 0;

protected:
    CgraEngine* cgra;
};

class BusNetwork : public Network {
public:
    BusNetwork(CgraEngine* _cgra) : Network(_cgra) {}
    void sendToken(PeIdx pe, TokenStore::Token tok);
};

}
}
}
