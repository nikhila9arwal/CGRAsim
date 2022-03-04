
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
    virtual bool sendToken(PeIdx pe, TokenStore::Token tok) = 0;

protected:
    CgraEngine* cgra;
};

class BusNetwork : public Network {
public:
    BusNetwork(CgraEngine* _cgra) : Network(_cgra) {}
    bool sendToken(PeIdx pe, TokenStore::Token tok) {
        if (!cgra->setToken(pe, tok)) {
            uint32_t newEventTime = currentTime + networkDelay + setTokenFailDelay;
            SendTokenCgraEvent event(newEventTime, pe, tok);
            pq.push(event);
        }
    }
};

}
}
}
