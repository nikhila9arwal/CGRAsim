#include "cgra_network.h"
#include "cgra_pe.h"
#include "cgra.h"

namespace platy {
namespace sim {
namespace cgra {

void BusNetwork::sendToken(PeIdx peid, TokenStore::Token tok) {
    ProcessingElement * pe = cgra->getProcessingElement(peid);
    if (!pe->acceptToken(tok)) {
        qassert(false);
        Cycles newEventTime = cgra->currentTime + cgra->networkDelay + cgra->setTokenFailDelay;
        CgraEvent* event =  new SendTokenCgraEvent(newEventTime, peid, tok);
        cgra->pushEvent(event);
    }
}

}
}
}
