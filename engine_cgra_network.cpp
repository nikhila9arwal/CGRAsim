#include "engine_cgra_network.h"

namespace platy {
namespace sim {
namespace ms {

void BusNetwork::sendToken(PeIdx pe, TokenStore::Token tok) {
    if (!cgra->setToken(pe, tok)) {
        uint32_t newEventTime = currentTime + networkDelay + setTokenFailDelay;
        CgraEvent* event =  new SendTokenCgraEvent(newEventTime, pe, tok);
        cgra->pushToCgraQueue(event);
    }
}

}
}
}